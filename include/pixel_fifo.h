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
  ReplaceFetchStrategy = 0,
  AppendFetchStrategy,
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

  void StartSpriteFetch(Sprite sprite);
  Sprite *row_sprites_;
  int row_sprites_count_;
  // Which index are we due to potentially fetch next.
  int sprite_index_ = 0;

  bool window_triggered_ = false;
  int window_x_ = 0;

  void OverlaySpriteFetch(int i);

  void StartFetch();
  void StartBackgroundFetch();
  void StartWindowFetch();
  void ApplyFetch();
  void PopFront();
  Pixel PeekFront();
  void Append(Pixel p);
  int SpriteIndexForX(int x);

  void ClearFifo();

  uint16_t BackgroundWindowTile(int x, int y, uint16_t tile_map_address_base);

 public:
  PixelFIFO(PPU *ppu);
  ~PixelFIFO();

  // Starts the new row 0->144.
  void NewRow(int row, Sprite *row_sprites, int row_sprites_count);
  bool Advance(Screen *screen);
  bool WindowTriggered() { return window_triggered_; }
};
