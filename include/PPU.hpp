#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Sprite.hpp"

class InterruptHandler;
class PixelFIFO;
class Screen;
struct Sprite;

using namespace std;

enum PPUState
{
    Unknown = 0,
    OAM_Search,
    Pixel_Transfer,
    HBlank,
    VBlank,
};

class PPU
{
public:
    PPU();
    ~PPU() = default;

    void Advance(int cycles);

    PPUState State() { return state_; };

    uint8_t GetByteAt(uint16_t address);
    void SetByteAt(uint16_t address, uint8_t byte);

    uint16_t BackgroundTile(int tile_x, int tile_y);
    uint16_t WindowTile(int x, int y);

	// Sprite, y is 0-sprite_height.
	uint16_t SpritePixels(Sprite sprite, int sprite_y);

    uint8_t scx();
    uint8_t scy();

	void SetInterruptHandler(InterruptHandler *handler) { interrupt_handler_ = handler; };

private:
    uint8_t *oam_ram_;
    uint8_t *video_ram_;
    uint8_t *io_ram_;
    Screen *screen_;
    PixelFIFO *fifo_;
	InterruptHandler* interrupt_handler_;

    PPUState state_;

	// How many cycles have been done in a frame for the 4.19 MHZ GPU.
    int frame_cycles_ = 0;
	// How many cycles we are advancing for the 4.19 MHZ GPU.
	int advance_cycles_ = 0;
	void AdvanceFrame(int frame_cycles);
    

	Sprite *row_sprites_;

    void VisibleCycle(int clockCycles);
    void InvisibleCycle(int clockCycles);
    void DrawRow(int row);

    void BeginHBlank();
    void EndHBlank();

    void BeginVBlank();
    void EndVBlank();

    bool CanAccessVRAM();
    bool CanAccessOAM();

    void SetIORAM(uint16_t address, uint8_t value);
    uint8_t GetIORAM(uint16_t address);

    void set_lcdc(uint8_t value);
    uint8_t lcdc();

    void set_stat(uint8_t value);
    uint8_t stat();

    void set_scx(uint8_t value);

	void set_scy(uint8_t value);
	uint8_t scy_;

    void set_ly(uint8_t value);
    uint8_t ly();

    void set_lyc(uint8_t value);
    uint8_t lyc();

    void set_wy(uint8_t value);
    uint8_t wy();

    void set_wx(uint8_t value);
    uint8_t wx();

    uint8_t bgp();
    uint8_t obp0();
    uint8_t obp1();

    void set_bgp(uint8_t address);
    void set_obp0(uint8_t address);
    void set_obp1(uint8_t address);

    bool DisplayWindow();

    // Performs the OAM Search function, finding up to 10 sprites visible at this row.
    void OAMSearchY(int row);
};
