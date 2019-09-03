#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Sprite.hpp"

using namespace std;

enum PPUState {
    Unknown = 0,
    OAM_Search,
    Pixel_Transfer,
    HBlank,
    VBlank,
};

class PPU {
    public:
        PPU();
        ~PPU() = default;

        void Advance(int cycles);

        PPUState State() { return state_; };

        uint8_t GetByteAt(uint16_t address);
        void SetByteAt(uint16_t address, uint8_t byte);

    private:
        uint8_t *oam_ram_;
        uint8_t *video_ram_;
        uint8_t *io_ram_;

        PPUState state_;
        int cycles_;
        vector<Sprite *> *row_sprites;

        void VisibleCycle();
        void InvisibleCycle();
        void DrawRow(int row);

        void BeginHBlank(int row);
        void EndHBlank();

        void BeginVBlank();
        void EndVBlank();

        bool CanAccessVRAM();
        bool CanAccessOAM();

        void set_scx(uint8_t value);
        uint8_t scx();

        void set_scy(uint8_t value);
        uint8_t scy();

        void set_ly(uint8_t value);
        uint8_t ly();

        // Performs the OAM Search function, returning up to 10 sprites visible on this row.
        vector<Sprite *> *OAMSearchY(int row);
};
