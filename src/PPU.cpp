#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "PPU.hpp"
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

PPU::PPU() { 
    oam_ram_ = (uint8_t *)calloc(0xA0, sizeof(uint8_t));
    video_ram_ = (uint8_t *)calloc(0x2000, sizeof(uint8_t));
    io_ram_ = (uint8_t *)calloc(0xD, sizeof(uint8_t));

    cycles_ = 0;
    state_ = OAM_Search;
    row_sprites = NULL;
}

void PPU::Advance(int cycles) {
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

    if (row_cycles == 0) {
        EndHBlank();
        state_ = OAM_Search;
        row_sprites = OAMSearchY(row);
    } else if (row_cycles == OAM_SEARCH_CYCLES) {
        state_ = Pixel_Transfer;
        DrawRow(row);
    } else if (row_cycles == OAM_SEARCH_CYCLES + PIXEL_TRANSFER_CYCLES) {
        state_ = HBlank;
        BeginHBlank(row);
    }
}

uint8_t PPU::scx() {
    return GetByteAt(0xFF43);
}

void PPU::set_scx(uint8_t value) {
    assert(false);
    SetByteAt(0xFF43, value);
}

uint8_t PPU::scy() {
    return GetByteAt(0xFF42);
}

void PPU::set_scy(uint8_t value) {
    SetByteAt(0xFF42, value);
}

uint8_t PPU::ly() {
    return GetByteAt(0xFF44);
}

void PPU::set_ly(uint8_t value) {
    SetByteAt(0xFF44, value);
    // cout << "LY == " << hex << unsigned(GetByteAt(0xFF44)) << endl;
}

uint8_t PPU::GetByteAt(uint16_t address) {
    if (address >= 0x8000 && address < 0xA000) {
        return video_ram_[address - 0x8000];
    } else if (address >= 0xFE00 && address < 0xFEA0) {
        return oam_ram_[address - 0xFE00];
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
        return io_ram_[address - 0xFF40];
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
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
        io_ram_[address - 0xFF40] = byte;
    } else {
        cout << "Unknown SET PPU address: 0x" << hex << unsigned(address) << endl;
        assert(false);
    }
}

void PPU::DrawRow(int row) {
    assert(scx() == 0);
    set_ly(row);
    int y = (row + scy()) % 144;
}

void PPU::BeginHBlank(int row) {
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

int SpriteHeight() {
    return 8;
}

bool PPU::CanAccessOAM() {
    return state_ == HBlank || state_ == VBlank;
}

bool PPU::CanAccessVRAM() {
    // TODO Writes are nops, reads are 0xFF.
    return state_ != Pixel_Transfer;
}

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
