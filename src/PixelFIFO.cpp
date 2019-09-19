#include "PixelFIFO.hpp"

#include <cassert>
#include <iostream>

#include "PPU.hpp"
#include "Screen.hpp"
#include "Utils.hpp"

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

void PixelFIFO::NewRow(int row) {
    if (fifo_ != NULL) {
        free(fifo_);
        fifo_ = NULL;
    }
    Reset();
    row_ = row;
    y_ = row_ + ppu_->scy();
    scx_shift_ = ppu_->scx() % 8;
    x_ = ppu_->scx() - scx_shift_;
    assert(pixels_outputted_ == 160 || pixels_outputted_ == 0);
    pixels_outputted_ = 0;
}

void PixelFIFO::PopFront() {
    fifo_start_++;
	fifo_start_ = fifo_start_ % 16;
    fifo_length_--;
}

Pixel PixelFIFO::PeekFront() {
    return fifo_[fifo_start_];
}

void PixelFIFO::Append(Pixel pixel) {
    int pos = (fifo_start_ + fifo_length_) % 16;
    fifo_[pos] = pixel;
    fifo_length_ += 1;
}

bool PixelFIFO::Advance(Screen *screen) {
	assert(pixels_outputted_ < SCREEN_WIDTH);
	if (row_ + ppu_->scy() != y_) {
		cout << "SCY Changed unexpectedly. row: " << hex << unsigned(row_);
		cout << " scy: " << hex << unsigned(ppu_->scy());
		cout << " != y: " << hex << unsigned(y_) << endl;
		assert(false);
	}
    if (fetch_->cycles_remaining_ > 0) {
        fetch_->cycles_remaining_--;
        if (fetch_->cycles_remaining_ == 0) {
            ApplyFetch();
        }
    }
    if (fifo_length_ <= 8 && fetch_->cycles_remaining_ == 0) {
        StartFetch();
    }

    if (!fifo_length_) {
        return false;
    }

    if (scx_shift_) {
        scx_shift_--;
        PopFront();
        x_++;
        return false;
    }

    screen->DrawPixel(PeekFront());
    PopFront();
    x_++;
    return (++pixels_outputted_ == SCREEN_WIDTH);
}

void PixelFIFO::ApplyFetch() {
    assert(fetch_->cycles_remaining_ == 0);

    switch(fetch_->strategy_) {
        case AppendFetchStrategy:
            assert(fifo_length_ <= 8);
            for (int i = 0; i < 8; i++) {
				Append(fetch_->pixels_[i]);
            }
            assert(fifo_length_ != 0);
            break;
        case OverlayFirst8FetchStrategy:
            cout << "TODO: Sprite!" << endl;
            assert(false);
            break;
        case ReplaceFetchStrategy:
            cout << "TODO: Replace Fetch" << endl;
            assert(false);
            break;
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
    fetch_->cycles_remaining_ = FETCH_CYCLES;
    
    // TODO: Find the right next tile based on where we are.
    int next_tile = x_ + fifo_length_;
    // cout << "Fetching next tile: 0x" << hex << unsigned(next_tile) << endl;
    uint16_t background_tile = ppu_->BackgroundTile(next_tile, y_);
    PixelList(background_tile, BackgroundWindowPalette, fetch_->pixels_);
    fetch_->strategy_ = AppendFetchStrategy;
}
