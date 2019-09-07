#pragma once

#include <list>

#include "Palette.hpp"
#include "Pixel.hpp"

using namespace std;

enum FetchStrategy {
    ReplaceFetchStrategy = 0,
    AppendFetchStrategy,
    OverlayFirst8FetchStrategy,
};

struct Fetch
{
    Pixel *pixels_;
    int cycles_remaining_;
    // Whether we should overlay the front 8 pixels for a sprite.
    FetchStrategy strategy_;
};

class PPU;
class Screen;

class PixelFIFO {
 private:
    Pixel *fifo_;
    int fifo_length_ = 0;
    int fifo_start_ = 0;

    PPU *ppu_;
    int scx_shift_ = 0;
    int pixels_outputted_ = 0;
    int x_ = 0;
    int y_ = 0;
    int row_ = 0;

    Fetch *fetch_ = NULL;

    void StartFetch();
    void ApplyFetch();
    void Reset();
    void PopFront();
    Pixel PeekFront();
	void Append(Pixel p);

 public:
    PixelFIFO(PPU *ppu);
    ~PixelFIFO() = default;

    // Starts the new row 0->144.
    void NewRow(int row);
    bool Advance(Screen *screen);
};