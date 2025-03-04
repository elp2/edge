#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "sprite.h"

class InterruptHandler;
class PixelFIFO;
class Screen;
struct Sprite;

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

  uint16_t BackgroundTile(int tile_x, int tile_y);
  uint16_t WindowTile(int x);

  // Sprite, y is 0-sprite_height.
  uint16_t SpritePixels(Sprite sprite, int sprite_y);

  uint8_t scx();
  uint8_t scy();

  void SetInterruptHandler(InterruptHandler *handler) {
    interrupt_handler_ = handler;
  };

  void SkipBootROM();

  bool WindowEnabledAt(int x, int y);

 private:
  uint8_t *oam_ram_ = NULL;
  uint8_t *video_ram_ = NULL;
  uint8_t *io_ram_ = NULL;
  Screen *screen_ = NULL;
  PixelFIFO *fifo_ = NULL;
  InterruptHandler *interrupt_handler_ = NULL;

  PPUState state_;

  // How many cycles have been done in a frame for the 4.19 MHZ GPU.
  int frame_cycles_ = 0;
  // How many cycles we are advancing for the 4.19 MHZ GPU.
  int advance_cycles_ = 0;
  void AdvanceFrame(int frame_cycles);

  Sprite *row_sprites_ = NULL;
  int row_sprites_count_ = 0;

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
  uint8_t scy_ = 0;

  void set_ly(uint8_t value);
  uint8_t ly();

  void set_lyc(uint8_t value);
  uint8_t lyc();

  void set_wy(uint8_t value);
  uint8_t wy();

  void SetWXPlus7(uint8_t value);
  uint8_t GetWXPlus7();

  // WX with the adjustment applied since wx = 7 is pixel 0.
  uint8_t WXPixelX() { return GetWXPlus7() - 7; }

  uint8_t bgp();
  uint8_t obp0();
  uint8_t obp1();

  void set_bgp(uint8_t address);
  void set_obp0(uint8_t address);
  void set_obp1(uint8_t address);

  // Performs the OAM Search function, finding up to 10 sprites visible at this
  // row.
  void OAMSearchY(int row);

  int SpriteHeight();

  uint16_t ReverseTileRow(uint16_t tile_row);

  bool BackgroundWindowEnablePriority();

  bool BackgroundTileMapHigh();
  bool WindowTileMapHigh();
  bool BackgroundWindowTileDataAreaLow();

  // The next window line to render is only incremented when we actually render a window,
  // not just versus the WY register.
  int window_render_line_ = 0;
};
