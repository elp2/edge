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
    list<Pixel> pixels_;
    int cycles_remaining_;
    // Whether we should overlay the front 8 pixels for a sprite.
    FetchStrategy strategy_;
};

class PPU;
class Screen;

class PixelFIFO {
 private:
    list<Pixel> *fifo_;
    PPU *ppu_;
    int scx_shift_;
    int pixels_outputted_;
    int x_;
    int y_;
    int row_;

    Fetch *fetch_ = NULL;

    void StartFetch();
    void ApplyFetch();

 public:
    PixelFIFO(PPU *ppu);
    ~PixelFIFO() = default;

    // Starts the new row 0->144.
    void NewRow(int row);
    bool Advance(Screen *screen);
};