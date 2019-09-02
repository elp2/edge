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

    private:
        uint8_t *oam_ram_;
        uint8_t *video_ram_;

        PPUState state_;
        int cycles_;
        vector<Sprite *> *row_sprites;

        void VisibleCycle();
        void DrawRow(int row);

        void BeginHBlank(int row);
        void EndHBlank();

        void BeginVBlank();
        void EndVBlank();

        bool CanAccessVRAM();
        bool CanAccessOAM();

        // Performs the OAM Search function, returning up to 10 sprites visible on this row.
        vector<Sprite *> *OAMSearchY(int row);
};
