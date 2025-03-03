#include "ppu.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "interrupt_controller.h"
#include "pixel_fifo.h"
#include "screen.h"
#include "sprite.h"
#include "utils.h"

using namespace std;

const int OAM_SEARCH_CYCLES = 20 * 4;
const int PIXEL_TRANSFER_CYCLES = 43 * 4;
const int HBLANK_CYCLES = 51 * 4;
const int ROW_CYCLES =
    OAM_SEARCH_CYCLES + PIXEL_TRANSFER_CYCLES + HBLANK_CYCLES;

const int ROWS = 144;
const int VISIBLE_CYCLES = ROWS * ROW_CYCLES;

const int VBLANK_ROWS = 10;
const int VBLANK_CYCLES = VBLANK_ROWS * ROW_CYCLES;

const int TOTAL_FRAME_CYCLES = VISIBLE_CYCLES + VBLANK_CYCLES;

const uint16_t LCDC_ADDRESS = 0xFF40;
const uint16_t STAT_ADDRESS = 0xFF41;
const uint16_t SCY_ADDRESS = 0xFF42;
const uint16_t SCX_ADDRESS = 0xFF43;
const uint16_t LY_ADDRESS = 0xFF44;
const uint16_t LYC_ADDRESS = 0xFF45;
const uint16_t BGP_ADDRESS = 0xFF47;
const uint16_t OBP0_ADDRESS = 0xFF48;
const uint16_t OBP1_ADDRESS = 0xFF49;
const uint16_t WY_ADDRESS = 0xFF4A;
const uint16_t WX_ADDRESS = 0xFF4B;

const int TILES_PER_ROW = 32;
const int BYTES_PER_8X8_TILE = 16;

const uint16_t OAM_RAM_ADDRESS = 0xFE00;
const int NUM_OAM_SPRITES = 40;
const int OAM_SPRITE_BYTES = 4;  // Technically only uses the first 28 bits.

PPU::PPU() {
  oam_ram_ = (uint8_t *)calloc(0xA0, sizeof(uint8_t));
  video_ram_ = (uint8_t *)calloc(0x2000, sizeof(uint8_t));
  for (int i = 0; i < 0x2000; i++) {
    // Initialize junk data so corruption will reveal errors.
    video_ram_[i] = i % 255;
  }
  io_ram_ = (uint8_t *)calloc(0xD, sizeof(uint8_t));

  frame_cycles_ = 0;
  state_ = OAM_Search;
  row_sprites_ = (Sprite *)calloc(10, sizeof(Sprite));
  screen_ = new Screen();
  fifo_ = new PixelFIFO(this);
}

void PPU::SkipBootROM() {
  SetIORAM(0xFF40, 0x91);
  SetIORAM(0xFF41, 0x85);
  SetIORAM(0xFF42, 0x00);
  SetIORAM(0xFF43, 0x00);
  SetIORAM(0xFF44, 0x00);
  SetIORAM(0xFF45, 0x00);
  SetIORAM(0xFF46, 0xFF);
  SetIORAM(0xFF47, 0xFC);
  SetIORAM(0xFF4A, 0x00);
  SetIORAM(0xFF4B, 0x00);
}

void PPU::Advance(int machine_cycles) {
  advance_cycles_ = machine_cycles;

  // Naive version - immediately do all the things for that particular cycle
  // once.
  while (advance_cycles_) {
    if (frame_cycles_ < VISIBLE_CYCLES) {
      int cycles_until_invis = VISIBLE_CYCLES - frame_cycles_;
      int max_cycles = min(cycles_until_invis, advance_cycles_);
      VisibleCycle(max_cycles);
    } else {
      int cycles_until_frame = TOTAL_FRAME_CYCLES - frame_cycles_;
      int max_cycles = min(cycles_until_frame, advance_cycles_);
      InvisibleCycle(max_cycles);
    }

    if (frame_cycles_ == TOTAL_FRAME_CYCLES) {
      frame_cycles_ = 0;
      EndVBlank();
    }
    assert(frame_cycles_ < TOTAL_FRAME_CYCLES);
  }
}

void PPU::AdvanceFrame(int frame_cycles) {
  advance_cycles_ -= frame_cycles;
  frame_cycles_ += frame_cycles;
  assert(frame_cycles <= TOTAL_FRAME_CYCLES);
  assert(advance_cycles_ >= 0);
}

void PPU::InvisibleCycle(int max_cycles) {
  if (frame_cycles_ == VISIBLE_CYCLES) {
    EndHBlank();
    BeginVBlank();
  }
  while (max_cycles) {
    int invisible_cycles = frame_cycles_ - VISIBLE_CYCLES;
    if (invisible_cycles % ROW_CYCLES == 0) {
      set_ly(invisible_cycles / ROW_CYCLES + ROWS);
    }
    AdvanceFrame(1);
    max_cycles--;
  }
}

void PPU::VisibleCycle(int remaining_cycles) {
  assert(remaining_cycles > 0);

  int row = frame_cycles_ / ROW_CYCLES;
  int row_cycles = frame_cycles_ % ROW_CYCLES;
  int max_cycles = min(remaining_cycles, ROW_CYCLES - row_cycles);

  if (row_cycles == 0) {
    EndHBlank();
    state_ = OAM_Search;
    OAMSearchY(row);
    set_ly(row);
    fifo_->NewRow(row, row_sprites_, row_sprites_count_);
  }

  if (row_cycles < OAM_SEARCH_CYCLES) {
    int oam_progress = min(OAM_SEARCH_CYCLES - row_cycles, max_cycles);

    max_cycles -= oam_progress;
    row_cycles += oam_progress;
    AdvanceFrame(oam_progress);
  }

  if (max_cycles > 0 && row_cycles == OAM_SEARCH_CYCLES) {
    state_ = Pixel_Transfer;
    fifo_->Advance(screen_);
  }

  while (max_cycles > 0 && state_ != HBlank) {
    if (fifo_->Advance(screen_)) {
      BeginHBlank();
    }

    max_cycles--;
    row_cycles++;
    AdvanceFrame(1);
  }

  assert(row_cycles != ROW_CYCLES);
  if (max_cycles > 0) {
    int hblank_progress = min(max_cycles, ROW_CYCLES - row_cycles);

    max_cycles -= hblank_progress;
    row_cycles += hblank_progress;
    AdvanceFrame(hblank_progress);
  }
  assert(max_cycles == 0);
}

void PPU::SetIORAM(uint16_t address, uint8_t value) {
  io_ram_[address - LCDC_ADDRESS] = value;
}

uint8_t PPU::GetIORAM(uint16_t address) {
  return io_ram_[address - LCDC_ADDRESS];
}

uint8_t PPU::scx() { return GetIORAM(SCX_ADDRESS); }

void PPU::set_scx(uint8_t value) { SetIORAM(SCX_ADDRESS, value); }

uint8_t PPU::scy() { return scy_; }

void PPU::set_scy(uint8_t value) {
  if (!CanAccessVRAM() && value != scy_) {
    cout << "Updating SCY during VRAM: " << hex << unsigned(scy_) << " -> "
         << hex << unsigned(value) << endl;
  }
  scy_ = value;
}

uint8_t PPU::ly() {
  // LY changes even when the screen is off so it's OK to read.
  return GetIORAM(LY_ADDRESS);
}

void PPU::set_ly(uint8_t value) {
  SetIORAM(LY_ADDRESS, value);
  if (value == lyc() && bit_set(stat(), 6)) {
    interrupt_handler_->RequestInterrupt(Interrupt_LCDC);
  }
}

void PPU::set_lyc(uint8_t value) { SetIORAM(LYC_ADDRESS, value); }

uint8_t PPU::lyc() { return GetIORAM(LYC_ADDRESS); }

void PPU::set_wy(uint8_t value) { SetIORAM(WY_ADDRESS, value); }

uint8_t PPU::wy() { return GetIORAM(WY_ADDRESS); }

void PPU::set_wx(uint8_t value) { SetIORAM(WX_ADDRESS, value); }

uint8_t PPU::wx() { return GetIORAM(WX_ADDRESS); }

void PPU::set_bgp(uint8_t value) {
  screen_->SetPalette(BackgroundWindowPalette, value);
  SetIORAM(BGP_ADDRESS, value);
}

void PPU::set_obp0(uint8_t value) {
  screen_->SetPalette(SpritePalette0, value);
  SetIORAM(OBP0_ADDRESS, value);
}

void PPU::set_obp1(uint8_t value) {
  screen_->SetPalette(SpritePalette1, value);
  SetIORAM(OBP1_ADDRESS, value);
}

uint8_t PPU::bgp() { return GetIORAM(BGP_ADDRESS); }

uint8_t PPU::obp0() { return GetIORAM(OBP0_ADDRESS); }

uint8_t PPU::obp1() { return GetIORAM(OBP1_ADDRESS); }

uint8_t PPU::lcdc() { return GetIORAM(LCDC_ADDRESS); }

void PPU::set_lcdc(uint8_t value) {
  bool screen_on = bit_set(value, 7);
  cout << "LCDC " << screen_on << " 0x" << hex << unsigned(value) << endl;
  screen_->set_on(screen_on);
  if (!screen_on && state_ != VBlank) {
    cout << "Turning off screen must happen in vblank but is " << hex << unsigned(state_) << endl;
  }

  SetIORAM(LCDC_ADDRESS, value);
}

uint8_t PPU::stat() {
  // Take bits 3-6 for the coincidence
  uint8_t stat = GetIORAM(STAT_ADDRESS) & 0x78;

  if (lyc() == ly()) {
    stat |= 0x4;
  }

  switch (state_) {
    case HBlank:
      stat |= 0x0;
      break;
    case VBlank:
      stat |= 0x1;
      break;
    case OAM_Search:
      stat |= 0x2;
      break;
    case Pixel_Transfer:
      stat |= 0x3;
      break;
    default:
      break;
  }

  return stat;
}

void PPU::set_stat(uint8_t value) { SetIORAM(STAT_ADDRESS, value); }

uint8_t PPU::GetByteAt(uint16_t address) {
  if (address >= 0x8000 && address < 0xA000) {
    return video_ram_[address - 0x8000];
  } else if (address >= 0xFE00 && address < 0xFEA0) {
    return oam_ram_[address - OAM_RAM_ADDRESS];
  } else if (address >= 0xFF40 && address < 0xFF4C) {
    switch (address) {
      case LCDC_ADDRESS:
        return lcdc();
      case STAT_ADDRESS:
        return stat();
      case SCY_ADDRESS:
        return scy();
      case SCX_ADDRESS:
        return scx();
      case LY_ADDRESS:
        return ly();
      case LYC_ADDRESS:
        return lyc();
      case BGP_ADDRESS:
        return bgp();
      case OBP0_ADDRESS:
        return obp0();
      case OBP1_ADDRESS:
        return obp1();
      case WY_ADDRESS:
        return wy();
      case WX_ADDRESS:
        return wx();
      default:
        cout << "Unknown GetByte " << hex << unsigned(address) << endl;
        assert(false);
        return 0;
    }
  } else {
    cout << "Unknown GET TPPU address: 0x" << hex << unsigned(address) << endl;
    assert(false);
    return 0x00;
  }
}

void PPU::SetByteAt(uint16_t address, uint8_t byte) {
  if (address >= 0x8000 && address < 0xA000) {
    if (screen_->on() && !CanAccessVRAM()) {
      // TODO - re enable this if relevant.x`
      // cout << "Can not access Video RAM during " << hex << unsigned(state_)
      // << endl;
    }
    video_ram_[address - 0x8000] = byte;
  } else if (address >= 0xFE00 && address < 0xFEA0) {
    if (!CanAccessOAM()) {
      cout << "Can not access OAM during " << hex << unsigned(state_) << endl;
    }
    oam_ram_[address - 0xFE00] = byte;
  } else if (address >= 0xFF40 && address < 0xFF4C) {
    switch (address) {
      case LCDC_ADDRESS:
        set_lcdc(byte);
        break;
      case STAT_ADDRESS:
        set_stat(byte);
        break;
      case SCY_ADDRESS:
        set_scy(byte);
        break;
      case SCX_ADDRESS:
        set_scx(byte);
        break;
      case LY_ADDRESS:
        cout << "Writing LY via memory to: 0x" << hex << unsigned(byte) << endl;
        // assert(false);
        set_ly(byte);
        break;
      case LYC_ADDRESS:
        set_lyc(byte);
        break;
      case BGP_ADDRESS:
        set_bgp(byte);
        break;
      case OBP0_ADDRESS:
        set_obp0(byte);
        break;
      case OBP1_ADDRESS:
        set_obp1(byte);
        break;
      case WY_ADDRESS:
        set_wy(byte);
        break;
      case WX_ADDRESS:
        set_wx(byte);
        break;
      default:
        cout << "Unknown GetByte " << hex << unsigned(address) << endl;
        assert(false);
        return;
    }
  } else {
    cout << "Unknown SET PPU address: 0x" << hex << unsigned(address) << endl;
    assert(false);
  }
}

void PPU::BeginHBlank() {
  screen_->NewLine();
  state_ = HBlank;
  if (bit_set(stat(), 3)) {
    interrupt_handler_->RequestInterrupt(Interrupt_LCDC);
  }
}

void PPU::EndHBlank() {}

void PPU::BeginVBlank() {
  state_ = VBlank;
  screen_->VBlankBegan();
  interrupt_handler_->RequestInterrupt(Interrupt_VBlank);
  if (bit_set(stat(), 4)) {
    interrupt_handler_->RequestInterrupt(Interrupt_LCDC);
  }
}

void PPU::EndVBlank() { screen_->VBlankEnded(); }

bool PPU::CanAccessOAM() { return state_ == HBlank || state_ == VBlank; }

bool PPU::CanAccessVRAM() {
  // TODO Writes are nops, reads are 0xFF.
  return state_ != Pixel_Transfer;
}

bool PPU::DisplayWindow() { return 0x20 == (GetByteAt(LCDC_ADDRESS) & 0x20); }

void PPU::OAMSearchY(int row) {
  if (bit_set(stat(), 5)) {
    interrupt_handler_->RequestInterrupt(Interrupt_LCDC);
  }

  if (!(bit_set(lcdc(), 1))) {
    // OBJ (Sprites) disabled.
    row_sprites_count_ = 0;
    return;
  }

  int sprites_found = 0;
  for (int i = 0; i < NUM_OAM_SPRITES; i++) {
    int offset = 4 * i;

    int sprite_y = oam_ram_[offset + 0];
    int sprite_x = oam_ram_[offset + 1];

    if ((sprite_x == 0 && sprite_y == 0)) {
      continue;
    }
    // Transform sprite origin to be top left from bottom right 8x16.
    sprite_x -= 8;
    sprite_y -= 16;
    if (SpriteYIntersectsRow(sprite_y, row, SpriteHeight())) {
      assert(sprite_y - row < SpriteHeight());

      row_sprites_[sprites_found].x_ = sprite_x;
      row_sprites_[sprites_found].y_ = sprite_y;
      row_sprites_[sprites_found].tile_number_ = oam_ram_[offset + 2];
      row_sprites_[sprites_found].flags_ = oam_ram_[offset + 3];
      sprites_found++;
    }
    if (sprites_found == 10) {
      break;
    }
  }
  row_sprites_count_ = sprites_found;
}

uint16_t PPU::BackgroundTile(int x, int y) {
  assert(x % 8 == 0);
  // cout << "Getting bg tile: 0x" << hex << unsigned(tile_x) << " x 0x" << hex
  // << unsigned(tile_y) << endl;
  int tile_map_x = (x / 8) % TILES_PER_ROW;
  int tile_map_y = (y / 8) % TILES_PER_ROW;

  int tile_index = (tile_map_x + tile_map_y * TILES_PER_ROW);
  uint16_t tile_map_address_base = bit_set(lcdc(), 3) ? 0x9C00 : 0x9800;
  uint16_t tile_map_address_ = tile_map_address_base + tile_index;
  uint8_t tile_number = GetByteAt(tile_map_address_);

  uint16_t tile_data_address;
  uint16_t tile_data_base_address = bit_set(lcdc(), 4) ? 0x8000 : 0x8800;
  int8_t signed_tile_number;
  switch (tile_data_base_address) {
    case 0x8000:
      tile_data_address =
          tile_data_base_address + tile_number * BYTES_PER_8X8_TILE;
      break;
    case 0x8800:
      signed_tile_number = tile_number;
      tile_data_address = 0x9000 + signed_tile_number * BYTES_PER_8X8_TILE;
      break;
    default:
      assert(false);
      break;
  }
  // Each row in title is two bytes.
  tile_data_address += (y % 8) * 2;
  uint16_t tile_data = buildMsbLsb16(GetByteAt(tile_data_address),
                                     GetByteAt(tile_data_address + 1));
  return tile_data;
}

uint16_t PPU::SpritePixels(Sprite sprite, int sprite_row) {
  if (sprite_row < 0 || sprite_row >= SpriteHeight()) {
    cout << "Sprite row out of range: " << sprite_row << endl;
    assert(false);
  }

  if (SpriteFlippedY(sprite)) {
    sprite_row = SpriteHeight() - sprite_row - 1;
  }

  uint16_t sprite_tile_address =
      0x8000 + sprite.tile_number_ * BYTES_PER_8X8_TILE;
  sprite_tile_address += (sprite_row % 8) * 2;
  if (sprite_row > 7) {
    sprite_tile_address += BYTES_PER_8X8_TILE;
  }
  uint16_t tile_row = buildMsbLsb16(GetByteAt(sprite_tile_address),
                                    GetByteAt(sprite_tile_address + 1));

  if (SpriteFlippedX(sprite)) {
    return ReverseTileRow(tile_row);
  } else {
    return tile_row;
  }
}

uint16_t PPU::WindowTile(int x, int y) {
  (void)x;
  (void)y;
  uint16_t window_map_address_base = bit_set(lcdc(), 6) ? 0x9800 : 0x9C00;
  // TODO: Windows not implemented.
  assert(false);
  return GetByteAt(window_map_address_base);
}

int PPU::SpriteHeight() { return bit_set(lcdc(), 2) ? 16 : 8; }

uint16_t PPU::ReverseTileRow(uint16_t tile_row) {
  // Process each byte separately
  uint16_t top = HIGHER8(tile_row);
  uint16_t bottom = LOWER8(tile_row);

    // Reverse bits in each byte.
    top = ((top & 0xF0) >> 4) | ((top & 0x0F) << 4);  // Swap nibbles
    top = ((top & 0xCC) >> 2) | ((top & 0x33) << 2);  // Swap 2-bit pairs
    top = ((top & 0xAA) >> 1) | ((top & 0x55) << 1);  // Swap adjacent bits
    
    bottom = ((bottom & 0xF0) >> 4) | ((bottom & 0x0F) << 4);
    bottom = ((bottom & 0xCC) >> 2) | ((bottom & 0x33) << 2);
    bottom = ((bottom & 0xAA) >> 1) | ((bottom & 0x55) << 1);
    
    return (top << 8) | bottom;
}
