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
    oam_ram_ = new uint8_t[0xa0];
    video_ram_ = new uint8_t[0x2000];
    io_ram_ = new uint8_t[0xd];
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
        } else if (cycles_ == VISIBLE_CYCLES) {
            EndHBlank();
            BeginVBlank();
        }

        cycles_++;
        cycles--;
        if (cycles_ == FRAME_CYCLES) {
            cycles_ = 0;
            EndVBlank();
        }        
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
        return 0xed;
    }
}

void PPU::SetByteAt(uint16_t address, uint8_t byte) {
    if (address >= 0x8000 && address < 0xA000) {
        video_ram_[address - 0x8000] = byte;
    } else if (address >= 0xFE00 && address < 0xFEA0) {
        oam_ram_[address - 0xFE00] = byte;
    } else if (address >= 0xFF40 && address <= 0xFF4C) {
        io_ram_[address - 0xFF40] = byte;
    } else {
        cout << "Unknown SET PPU address: 0x" << hex << unsigned(address) << endl;
        assert(false);
    }
}

void PPU::DrawRow(int row) {
    // TODO Draw a row :O.
    cout << "Drawing row: " << row << endl;
}

void PPU::BeginHBlank(int row) {
    // TODO HBlank.
    cout << "HBlank for " << row << endl;
}

void PPU::EndHBlank() {
    // TODO HBlank End.
    cout << "HBlank ended!!!" << endl;
}

void PPU::BeginVBlank() {
    state_ = VBlank;
    // TODO VBlank begin.
    cout << "Vblank began!" << endl;
}

void PPU::EndVBlank() {
    // TODO VBlank end.
    cout << "VBlank ended!" << endl;
}

int SpriteHeight() {
    return 8;
}

bool PPU::CanAccessOAM() {
    // TODO CanAccessOAM.
    return state_ == HBlank || state_ == VBlank;
}

bool PPU::CanAccessVRAM() {
    // TODO CanAccessVRAM.
    // Writes are nops, reads are 0xff.
    return state_ != Pixel_Transfer;
}

vector<Sprite *> *PPU::OAMSearchY(int row) {
    cout << "OAM Search!" << endl;
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
