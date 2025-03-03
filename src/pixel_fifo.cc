#include "pixel_fifo.h"

#include <cassert>
#include <cstddef>
#include <iostream>

#include "ppu.h"
#include "screen.h"
#include "utils.h"

const int FETCH_CYCLES = 3;

PixelFIFO::PixelFIFO(PPU *ppu) {
  ppu_ = ppu;
  Reset();
  fetch_ = new Fetch();
  fetch_->cycles_remaining_ = 0;
  fetch_->pixels_ = (Pixel *)calloc(8, sizeof(Pixel));
  fetch_->strategy_ = AppendFetchStrategy;
}

void PixelFIFO::Reset() {
  fifo_ = (Pixel *)calloc(16, sizeof(Pixel));
  fifo_length_ = 0;
  fifo_start_ = 0;
}

void PixelFIFO::NewRow(int pixely, Sprite *row_sprites, int row_sprites_count) {
  if (fifo_ != NULL) {
    free(fifo_);
    fifo_ = NULL;
  }
  Reset();

  pixelx_ = 0;
  pixely_ = pixely;
  row_sprites_ = row_sprites;
  row_sprites_count_ = row_sprites_count;
  scx_shift_ = ppu_->scx() % 8;
  bgx_ = ppu_->scx() - scx_shift_;

  assert(pixels_outputted_ == 160 || pixels_outputted_ == 0);
  pixels_outputted_ = 0;
}

void PixelFIFO::PopFront() {
  fifo_start_++;
  fifo_start_ = fifo_start_ % 16;
  fifo_length_--;
}

Pixel PixelFIFO::PeekFront() { return fifo_[fifo_start_]; }

void PixelFIFO::Append(Pixel pixel) {
  int pos = (fifo_start_ + fifo_length_) % 16;
  fifo_[pos] = pixel;
  fifo_length_ += 1;
}

bool PixelFIFO::Advance(Screen *screen) {
  assert(pixels_outputted_ < SCREEN_WIDTH);

  if (fetch_->cycles_remaining_ > 0) {
    fetch_->cycles_remaining_--;
    if (fetch_->cycles_remaining_ == 0) {
      ApplyFetch();
    }
  }
  StartFetch();
  if (fetch_->cycles_remaining_ > 0 &&
      (fetch_->strategy_ == OverlayFirst8FetchStrategy ||
       fetch_->strategy_ == ReplaceFetchStrategy)) {
    // Need to keep the fetch running.
    return false;
  }
  int sprite_i = SpriteIndexForX(pixelx_);
  if (fetch_->cycles_remaining_ > 0 && sprite_i != -1) {
    // BG fetch happening, but we also need to apply a sprite here.
    return false;
  }

  if (!fifo_length_) {
    return false;
  }

  if (scx_shift_) {
    scx_shift_--;
    PopFront();
    return false;
  }

  screen->DrawPixel(PeekFront());
  PopFront();
  pixelx_++;
  sprite_index_ = 0;
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
      if (bit_set(fetch_->sprite_.flags_, 5)) {
        // X flipped, apply bits backwards.
        for (int i = 7; i >= 0; i--) {
          OverlaySpriteFetch(i);
        }
      } else {
        for (int i = 0; i < 8; i++) {
          OverlaySpriteFetch(i);
        }
      }
      break;
    case ReplaceFetchStrategy:
      cout << "TODO: Replace Fetch" << endl;
      assert(false);
      break;
  }
}

void PixelFIFO::OverlaySpriteFetch(int i) {
  Pixel sp = fetch_->pixels_[i];
  int pos = (fifo_start_ + i) % 16;
  Pixel fp = fifo_[pos];
  if (fp.palette_ == BackgroundWindowPalette) {
    // TODO: Priority.
    if (sp.two_bit_color_ != 0x00) {
      if (fp.palette_ == BackgroundWindowPalette) {
        fifo_[pos] = sp;
      }
    }
  }
}

void PixelList(uint16_t pixels, Palette palette, Pixel *list) {
  uint8_t row_a = HIGHER8(pixels);
  uint8_t row_b = LOWER8(pixels);
  for (int i = 7; i >= 0; i--) {
    list[i].palette_ = palette;
    list[i].two_bit_color_ = ((row_a & 0x1) << 1) | (row_b & 0x1);
    row_a >>= 1;
    row_b >>= 1;
  }
}

void PixelFIFO::StartFetch() {
  if (fetch_->cycles_remaining_ != 0) {
    // Fetch already active.
    return;
  }
  if (fifo_length_ <= 8) {
    StartBackgroundFetch();
  } else {
    int sprite_i = SpriteIndexForX(pixelx_);
    if (sprite_i == -1) {
      return;
    }
    StartSpriteFetch(row_sprites_[sprite_i]);
    // Next time don't consider this sprite again.
    sprite_index_++;
    return;
  }
}

// Returns the first sprite at x past sprite_index_, or -1 if there is no such
// sprite.
int PixelFIFO::SpriteIndexForX(int x) {
  for (int i = sprite_index_; i < row_sprites_count_; i++) {
    if (row_sprites_[i].x_ == x) {
      return i;
    }
  }
  return -1;
}

void PixelFIFO::StartSpriteFetch(Sprite sprite) {
  fetch_->cycles_remaining_ = FETCH_CYCLES;

  uint16_t sprite_row_pixels = ppu_->SpritePixels(sprite, pixely_ - sprite.y_);
  Palette p = bit_set(sprite.flags_, 4) ? SpritePalette1 : SpritePalette0;

  PixelList(sprite_row_pixels, p, fetch_->pixels_);
  fetch_->strategy_ = OverlayFirst8FetchStrategy;
  fetch_->sprite_ = sprite;
}

void PixelFIFO::StartBackgroundFetch() {
  fetch_->cycles_remaining_ = FETCH_CYCLES;
  // TODO: Sometimes chooses the next row's tile for first tile.
  bgx_ += fifo_length_;
  uint16_t background_tile = ppu_->BackgroundTile(bgx_, pixely_ + ppu_->scy());
  PixelList(background_tile, BackgroundWindowPalette, fetch_->pixels_);
  fetch_->strategy_ = AppendFetchStrategy;
}
