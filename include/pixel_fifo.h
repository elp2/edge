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
  int x_ = 0;
  int y_ = 0;
  int row_ = 0;

  Fetch *fetch_ = NULL;

  void StartSpriteFetch(Sprite sprite);
  Sprite *row_sprites_;
  // Which index are we due to potentially fetch next.
  int sprite_index_ = 0;
  void OverlaySpriteFetch(int i);

  void StartFetch();
  void StartBackgroundFetch();
  void ApplyFetch();
  void Reset();
  void PopFront();
  Pixel PeekFront();
  void Append(Pixel p);
  int SpriteIndexForX(int x);

 public:
  PixelFIFO(PPU *ppu);
  ~PixelFIFO() = default;

  // Starts the new row 0->144.
  void NewRow(int row, Sprite *row_sprites);
  bool Advance(Screen *screen);
};