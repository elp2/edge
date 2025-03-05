#pragma once

#include <cstddef>
#include <list>

#include "palette.h"
#include "pixel.h"
#include "sprite.h"

using namespace std;

class PPU;
class Screen;

enum FetchStrategy {
  AppendFetchStrategy = 0,
  OverlayFirst8FetchStrategy,
};

struct Fetch {
  Pixel *pixels_;
  int cycles_remaining_;
  // Whether we should overlay the front 8 pixels for a sprite.
  FetchStrategy strategy_;
  Sprite sprite_;
};

class PixelFIFO {
 private:
  Pixel *fifo_;
  int fifo_length_ = 0;
  int fifo_start_ = 0;

  PPU *ppu_;
  int scx_shift_ = 0;
  int pixels_outputted_ = 0;
 
  int bgx_ = 0;
 
  int pixelx_ = 0;
  int pixely_ = 0;

  Fetch *fetch_ = NULL;

  Sprite *row_sprites_;
  int row_sprites_count_;

  bool window_triggered_ = false;
  int window_x_ = 0;

  // What pixel to fetch sprites for. Starts < 0 to capture sprites off of the left edge.
  int sprite_fetch_x = -7;

  void OverlaySpriteFetch(int i);

  void StartBackgroundFetch();
  void StartWindowFetch();
  void StartSpriteFetch(Sprite sprite, bool immediately_apply, int left_shift);
  void ApplyFetch();

  Pixel PopFrontPixel();
  void Append(Pixel p);
  int FirstSpriteIndexForX(int x);

  void ClearFifo();

  uint16_t BackgroundWindowTile(int x, int y, uint16_t tile_map_address_base);

 public:
  PixelFIFO(PPU *ppu);
  ~PixelFIFO();

  // Starts the new row 0->143.
  void NewRow(int row, Sprite *row_sprites, int row_sprites_count);
  bool Advance(Screen *screen);
  bool WindowTriggered() { return window_triggered_; }
};
