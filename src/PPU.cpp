#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "PixelFIFO.hpp"
#include "PPU.hpp"
#include "Screen.hpp"
#include "Sprite.hpp"

using namespace std;

const int OAM_SEARCH_CYCLES = 20;
const int PIXEL_TRANSFER_CYCLES = 43;
const int HBLANK_CYCLES = 51;
const int ROW_CYCLES = OAM_SEARCH_CYCLES + PIXEL_TRANSFER_CYCLES + HBLANK_CYCLES;

const int ROWS = 144;
const int VISIBLE_CYCLES = ROWS * ROW_CYCLES;

const int VBLANK_ROWS = 10;
const int VBLANK_CYCLES = VBLANK_ROWS * ROW_CYCLES;

const int FRAME_CYCLES = VISIBLE_CYCLES + VBLANK_CYCLES;

const uint16_t LCDC_ADDRESS = 0xFF40;
const uint16_t STAT_ADDRESS = 0xFF41;
const uint16_t SCY_ADDRESS = 0xFF42;
const uint16_t SCX_ADDRESS = 0xFF43;
const uint16_t LY_ADDRESS = 0xFF44;
const uint16_t LYC_ADDRESS = 0xFF45;
const uint16_t DMA_ADDRESS = 0xFF46;
const uint16_t BGP_ADDRESS = 0xFF47;
const uint16_t OBP0_ADDRESS = 0xFF48;
const uint16_t OBP1_ADDRESS = 0xFF49;
const uint16_t WY_ADDRESS = 0xFF4A;
const uint16_t WX_ADDRESS = 0xFF4B;

PPU::PPU() { 
    oam_ram_ = (uint8_t *)calloc(0xA0, sizeof(uint8_t));
    video_ram_ = (uint8_t *)calloc(0x2000, sizeof(uint8_t));
    io_ram_ = (uint8_t*)calloc(0xD, sizeof(uint8_t));

    cycles_ = 0;
    state_ = OAM_Search;
    row_sprites = NULL;
    screen_ = new Screen();
    fifo_ = new PixelFIFO(this);
}

void PPU::Advance(int cycles) {
    if (!screen_->on()) {
        // Nothing for the PPU to output if the screen's not on.
        return;
    }
    // Naive version - immediately do all the things for that particular cycle once.
    while(cycles) {
        // Iterating one at a time should be OK since we shouldn't be jumping
        // more than 30 or so cycles at a time.

        if (cycles_ < VISIBLE_CYCLES) {
            VisibleCycle();
        } else {
            InvisibleCycle();
        }

        cycles_++;
        cycles--;
        if (cycles_ == FRAME_CYCLES) {
            cycles_ = 0;
            EndVBlank();
        }        
    }
}

void PPU::InvisibleCycle() {
    if (cycles_ == VISIBLE_CYCLES) {
        EndHBlank();
        BeginVBlank();
    }
    int invisible_cycles = cycles_ - VISIBLE_CYCLES;
    if (invisible_cycles % ROW_CYCLES == 0) {
        set_ly(invisible_cycles / ROW_CYCLES + 144);
    }
}

void PPU::VisibleCycle() {
    int row = cycles_ / ROW_CYCLES;
    int row_cycles = cycles_ % ROWS;

    if (state_ == HBlank) {
        return;
    }

    if (row_cycles == 0) {
        EndHBlank();
        state_ = OAM_Search;
        row_sprites = OAMSearchY(row);
    } else if (row_cycles < OAM_SEARCH_CYCLES) {
        // NOP.
    } else if (row_cycles == OAM_SEARCH_CYCLES) {
        state_ = Pixel_Transfer;
        fifo_->NewRow(row);
        fifo_->Advance(screen_);
    } else {
        if (state_ == HBlank) {
            // NOP.
        } else {
            // Keep advancing the FIFO until it signals that it's done.
            if (fifo_->Advance(screen_)) {
                state_ = HBlank;
            }
        }
    }
}

void PPU::SetIORAM(uint16_t address, uint8_t value) {
    io_ram_[address - LCDC_ADDRESS] = value;
}

uint8_t PPU::GetIORAM(uint16_t address) {
    return io_ram_[address - LCDC_ADDRESS];
}

uint8_t PPU::scx() {
    return GetIORAM(SCX_ADDRESS);
}

void PPU::set_scx(uint8_t value) { 
	SetIORAM(SCX_ADDRESS, value);
}

uint8_t PPU::scy() {
    return GetIORAM(SCY_ADDRESS);
}

void PPU::set_scy(uint8_t value) {
	SetIORAM(SCY_ADDRESS, value);
}

uint8_t PPU::ly() {
    return GetIORAM(LY_ADDRESS);
}

void PPU::set_ly(uint8_t value) {
	SetIORAM(LY_ADDRESS, value);
    // cout << "LY == " << hex << unsigned(GetByteAt(0xFF44)) << endl;
}

void PPU::set_lyc(uint8_t value) {
	SetIORAM(LYC_ADDRESS, value);
}

uint8_t PPU::lyc() {
    return GetIORAM(LYC_ADDRESS);
}

void PPU::set_dma(uint8_t value) {
	cout << "Attempted OAM DMA! TODO!" << endl;
    assert(false); // TODO.
	SetIORAM(DMA_ADDRESS, value);
}

uint8_t PPU::dma() {
    assert(false); // TODO. Does it even make sense?
    return GetIORAM(DMA_ADDRESS);
}

void PPU::set_wy(uint8_t value) {
	SetIORAM(WY_ADDRESS, value);
}

uint8_t PPU::wy() {
    return GetIORAM(WY_ADDRESS);
}

void PPU::set_wx(uint8_t value) {
	SetIORAM(WX_ADDRESS, value);
}

uint8_t PPU::wx() {
    return GetIORAM(WX_ADDRESS);
}

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

uint8_t PPU::bgp() {
	return GetIORAM(BGP_ADDRESS);
}

uint8_t PPU::obp0() {
	return GetIORAM(OBP0_ADDRESS);
}

uint8_t PPU::obp1() {
	return GetIORAM(OBP1_ADDRESS);
}

uint8_t PPU::lcdc() {
    return GetIORAM(LCDC_ADDRESS);
}

void PPU::set_lcdc(uint8_t value) {
    // Don't do pixel fifos + shit when the lcd's not on!
    // 10010001.
    bool screen_on = value & 80;
    screen_->set_on(screen_on);
    // window_tile_map_base_address_ = value & 0x40 ? 0x9C00 : 0x9800;
    // // TODO read window display 0x20.
    // background_tile_data_base_address_ = value & 0x10 ? 0x8000 : 0x8800;
    // background_tile_map_display_base_address = value &0x8 ? 0x9C00 : 0x9800;
    // Sprite height should be handled.
    // BG & window.
    // bool sprites = value & 0x2;
    // bool bg = value & 0x1

    

    if (value & 80) {

    }

	SetIORAM(LCDC_ADDRESS, value);
}

uint8_t PPU::stat() {
    return GetIORAM(STAT_ADDRESS);
}

void PPU::set_stat(uint8_t value) {
	SetIORAM(STAT_ADDRESS, value);
}

uint8_t PPU::GetByteAt(uint16_t address) {
    if (address >= 0x8000 && address < 0xA000) {
        return video_ram_[address - 0x8000];
    } else if (address >= 0xFE00 && address < 0xFEA0) {
        return oam_ram_[address - 0xFE00];
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
            case DMA_ADDRESS:
                return dma();
            case BGP_ADDRESS:
            case OBP0_ADDRESS:
            case OBP1_ADDRESS:
                // TODO Palettes.
                return 0xed;
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
        if (!CanAccessVRAM()) {
            // Too many apparent false positives from boot ROM.
            // TODO: Maybe this is not checked when screen is not on.
            // cout << "Can not access Video RAM during " << hex << unsigned(state_) << endl;
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
                set_ly(byte);
                break;
            case LYC_ADDRESS:
                set_lyc(byte);
                break;
            case DMA_ADDRESS:
                set_dma(byte);
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

void PPU::BeginHBlank(int row) {
    (void)row;
    // TODO HBlank.
}

void PPU::EndHBlank() {
    // TODO HBlank End.
}

void PPU::BeginVBlank() {
    state_ = VBlank;
    // TODO VBlank begin.
}

void PPU::EndVBlank() {
    // TODO VBlank end.
}

int PPU::SpriteHeight() {
    return 0x4 == (GetByteAt(LCDC_ADDRESS) & 0x4) ? 16 : 8;
}

bool PPU::CanAccessOAM() {
    return state_ == HBlank || state_ == VBlank;
}

bool PPU::CanAccessVRAM() {
    // TODO Writes are nops, reads are 0xFF.
    return state_ != Pixel_Transfer;
}

bool PPU::DisplaySprites() {
    return 0x2 == (GetByteAt(LCDC_ADDRESS) & 0x2);
}

bool PPU::DisplayWindow() {
    return 0x20 == (GetByteAt(LCDC_ADDRESS) & 0x20);
}

// TODO: BG & Window: 

vector<Sprite *> *PPU::OAMSearchY(int row) {
    const int NUM_SPRITES = 40;
    vector<Sprite *> *sprites = new vector<Sprite *>();

    for (int i = 0; i < NUM_SPRITES; i++) {
        int offset = 4 * i;
        Sprite *sprite = new Sprite(oam_ram_[offset], oam_ram_[offset + 1], oam_ram_[offset + 2], oam_ram_[offset + 3]);
        if (sprite->IntersectsRow(row, SpriteHeight())) {
            sprites->push_back(sprite);
        }
    }
    return sprites;
}

uint16_t PPU::BackgroundTile(int tile_x, int tile_y) {
    assert(tile_x % 8 == 0);
    (void)tile_y; // TODO remove.
    // cout << "Getting bg tile: 0x" << hex << unsigned(tile_x) << " x 0x" << hex << unsigned(tile_y) << endl;
    // int tile_map_x = (tile_x / 8) % 32;
    // int tile_map_y = (tile_y / 8) % 32;

    return 0xaaaa; // TODO actually get right bank, and right details.
}
