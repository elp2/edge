#include "pixel_fifo.h"

#include <cassert>
#include <cstddef>
#include <iostream>

#include "ppu.h"
#include "screen.h"
#include "utils.h"

const int FETCH_CYCLES = 3;
const int FIFO_LENGTH = 16;

PixelFIFO::PixelFIFO(PPU *ppu) {
  ppu_ = ppu; 
 
  fetch_ = new Fetch();
  fetch_->cycles_remaining_ = 0;
  fetch_->pixels_ = (Pixel *)calloc(8, sizeof(Pixel));
  fetch_->strategy_ = AppendFetchStrategy;
  
  fifo_ = (Pixel *)calloc(FIFO_LENGTH, sizeof(Pixel));
  ClearFifo();
}

PixelFIFO::~PixelFIFO() {
  free(fetch_->pixels_);
  free(fetch_);
  free(fifo_);
}

void PixelFIFO::ClearFifo() {
  fifo_length_ = 0;
  fifo_start_ = 0;
  for (int i = 0; i < FIFO_LENGTH; i++) {
    fifo_[i] = Pixel{0x00, BackgroundWindowPalette, false};
  }
}

void PixelFIFO::NewRow(int pixely, Sprite *row_sprites, int row_sprites_count) {
  ClearFifo();

  window_triggered_ = false;
  window_x_ = 0;

  pixelx_ = 0;
  pixels_outputted_ = 0;
  pixely_ = pixely;

  sprite_fetch_x = -7;
  row_sprites_ = row_sprites;
  row_sprites_count_ = row_sprites_count;

  scx_shift_ = ppu_->scx() % 8;
  bgx_ = ppu_->scx() - scx_shift_;
  assert(pixely_ < 144);
}

Pixel PixelFIFO::PopFrontPixel() {
  Pixel pixel = fifo_[fifo_start_];
  fifo_start_++;
  fifo_start_ = fifo_start_ % 16;
  fifo_length_--;
  return pixel;
}

void PixelFIFO::Append(Pixel pixel) {
  int pos = (fifo_start_ + fifo_length_) % 16;
  fifo_[pos] = pixel;
  fifo_length_ += 1;
}

bool PixelFIFO::Advance(Screen *screen) {
  assert(pixels_outputted_ < SCREEN_WIDTH);

  // Tick existing fetch, and apply if done.
  if (fetch_->cycles_remaining_ > 0) {
    fetch_->cycles_remaining_--;
    if (fetch_->cycles_remaining_ == 0) {
      ApplyFetch();
    } else {
      // We need to wait until the fetch completes.
      return false;
    }
  }

  // Always check to see if the window is enabled. May override prev fetch.
  bool new_window_triggered = ppu_->WindowEnabledAt(pixelx_, pixely_);
  if (new_window_triggered != window_triggered_) { 
    window_triggered_ = new_window_triggered;
    ClearFifo();
    // Kill any outstanding fetch. We will request sprites again if necessary.
    fetch_->cycles_remaining_ = 0;

    // Fetch sprites that might be spanning the window / background.
    sprite_fetch_x = pixelx_ - 7;

    if (!window_triggered_) {
      std::cout << "Unexpected: Window disabled during the line." << std::endl;
      assert(false);
    }
  }

  // As a top priority, the fifo must be 8 pixels so that sprites could be blended.
  if (fifo_length_ <= 8) {
    if (window_triggered_) {
      StartWindowFetch();
    } else {
      StartBackgroundFetch();
    }
    return false;
  }

  assert(fifo_length_ >= 8);

  // Now that we have items in the FIFO, shift out SCX.
  if (scx_shift_) {
    scx_shift_--;
    PopFrontPixel();
    return false;
  }

  // Now that BG/Window are ready, Overlay sprites.
  while (sprite_fetch_x <= pixelx_) {
    // Only consider sprites that could affect us.
    sprite_fetch_x = max(sprite_fetch_x, pixelx_ - 7);
    int sprite_i = FirstSpriteIndexForX(sprite_fetch_x);
    if (sprite_i != -1) {
      bool apply_immediately = 0 == (pixelx_ - sprite_fetch_x);
      StartSpriteFetch(row_sprites_[sprite_i], apply_immediately, pixelx_ - sprite_fetch_x);
    }
    sprite_fetch_x++;
  }

  if (fetch_->cycles_remaining_ > 0) {
    // We are fetching the sprite.
    assert(fetch_->strategy_ == OverlayFirst8FetchStrategy);
    return false;
  }

  screen->DrawPixel(PopFrontPixel());
  pixelx_++;

  return (++pixels_outputted_ == SCREEN_WIDTH);
}

void PixelFIFO::ApplyFetch() {
  assert(fetch_->cycles_remaining_ == 0);

  switch (fetch_->strategy_) {
    case AppendFetchStrategy:
      assert(fifo_length_ <= 8);
      for (int i = 0; i < 8; i++) {
        Append(fetch_->pixels_[i]);
      }
      assert(fifo_length_ != 0);
      break;
    case OverlayFirst8FetchStrategy:
      assert(fifo_length_ >= 8);
      for (int i = 0; i < 8; i++) {
        OverlaySpriteFetch(i);
      }
      break;
  }
}

void PixelFIFO::OverlaySpriteFetch(int i) {
  Pixel sprite_pixel = fetch_->pixels_[i];
  int pos = (fifo_start_ + i) % 16;
  Pixel fifo_pixel = fifo_[pos];
  if (fifo_pixel.palette_ == BackgroundWindowPalette) {
    if (sprite_pixel.sprite_over_background_window_ || fifo_pixel.two_bit_color_ == 0x00) {
      if (sprite_pixel.two_bit_color_ != 0x00) {
        fifo_[pos] = sprite_pixel;
      }
    }
  } else {
    // Sprite over sprite if existing pixel is transparent.
    if (fifo_pixel.two_bit_color_ == 0x00) {
      fifo_[pos] = sprite_pixel;
    }
  }
}

void PixelList(uint16_t pixels, Palette palette, Pixel *list, bool sprite_over_background_window) {
  uint8_t high = HIGHER8(pixels);
  uint8_t low = LOWER8(pixels);
  for (int i = 7; i >= 0; i--) {
    list[i].palette_ = palette;
    list[i].two_bit_color_ = ((high & 0x1)) | ((low & 0x1) << 1);
    list[i].sprite_over_background_window_ = sprite_over_background_window;
    high >>= 1;
    low >>= 1;
  }
}

// Returns the first sprite at x past sprite_index_, or -1 if there is no such
// sprite.
int PixelFIFO::FirstSpriteIndexForX(int x) {
  for (int i = 0; i < row_sprites_count_; i++) {
    if (row_sprites_[i].x_ == x) {
      return i;
    }
  }
  return -1;
}

void PixelFIFO::StartSpriteFetch(Sprite sprite, bool immediately_apply, int left_shift) {
  assert(fetch_->cycles_remaining_ == 0);

  uint16_t sprite_row_pixels = ppu_->SpritePixels(sprite, pixely_ - sprite.y_);
  Palette p = SpriteUsesPalette1(sprite) ? SpritePalette1 : SpritePalette0;

  PixelList(sprite_row_pixels, p, fetch_->pixels_, SpriteOverBackgroundWindow(sprite));
  if (left_shift) {
    for (int x = 0; x < 8; x++) {
      if (x + left_shift < 8) {
        fetch_->pixels_[x] = fetch_->pixels_[x + left_shift];
      }  else {
        fetch_->pixels_[x] = Pixel{0x0, SpritePalette0, false};
      }
    }
  }

  fetch_->strategy_ = OverlayFirst8FetchStrategy;
  fetch_->sprite_ = sprite;
  if (immediately_apply) {
    fetch_->cycles_remaining_ = 0;
    ApplyFetch();
  } else {
    fetch_->cycles_remaining_ = FETCH_CYCLES;
  }
}

void PixelFIFO::StartBackgroundFetch() {
  if (fetch_->cycles_remaining_ > 0) {
    return;
  }
  fetch_->cycles_remaining_ = FETCH_CYCLES;
  // TODO: Sometimes chooses the next row's tile for first tile. #18.
  bgx_ += fifo_length_;
  uint16_t background_tile = ppu_->BackgroundTile(bgx_, pixely_ + ppu_->scy());
  PixelList(background_tile, BackgroundWindowPalette, fetch_->pixels_, false);
  fetch_->strategy_ = AppendFetchStrategy;
}

void PixelFIFO::StartWindowFetch() {
  if (fetch_->cycles_remaining_ > 0) {
    return;
  }
  fetch_->cycles_remaining_ = FETCH_CYCLES;
  uint16_t window_tile = ppu_->WindowTile(window_x_);
  PixelList(window_tile, BackgroundWindowPalette, fetch_->pixels_, false);
  fetch_->strategy_ = AppendFetchStrategy;
  window_x_ += 8;
}
