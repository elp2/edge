#include "PixelFIFO.hpp"

#include <iostream>

#include "PPU.hpp"
#include "Screen.hpp"
#include "Utils.hpp"

const int PIXELS_PER_ROW = 160;
const int FETCH_CYCLES = 3;

PixelFIFO::PixelFIFO(PPU *ppu) {
    ppu_ = ppu;
    fifo_ = new list<Pixel>();
    fetch_ = NULL;
}

void PixelFIFO::NewRow(int row) {
    if (fifo_ != NULL) {
        free(fifo_);
        fifo_ = NULL;
    }
    fifo_ = new list<Pixel>();
    row_ = row;
    y_ = row_ + ppu_->scy();
    scx_shift_ = ppu_->scx() % 8;
    x_ = ppu_->scx() - scx_shift_;
    assert(pixels_outputted_ == 160 || pixels_outputted_ == 0);
    pixels_outputted_ = 0;
}

bool PixelFIFO::Advance(Screen *screen) {
    assert(row_ + ppu_->scy() == y_);
    if (fetch_) {
        fetch_->cycles_remaining_--;
        if (fetch_->cycles_remaining_ == 0) {
            ApplyFetch();
        }
    }
    if (fifo_->size() <= 8 && fetch_ == NULL) {
        StartFetch();
    }

    if (!fifo_->size()) {
        return false;
    }

    if (scx_shift_) {
        scx_shift_--;
        fifo_->pop_front();
        x_++;
        return false;
    }

    screen->DrawPixel(fifo_->front());
    fifo_->pop_front();
    x_++;
    return (++pixels_outputted_ == PIXELS_PER_ROW);
}

void PixelFIFO::ApplyFetch() {
    assert(fetch_->cycles_remaining_ == 0);

    switch(fetch_->strategy_) {
        case AppendFetchStrategy:
            assert(fifo_->size() <= 8);
            assert(fetch_->pixels_.size() == 8);
            fifo_->splice(fifo_->end(), fetch_->pixels_);
            assert(fifo_->size() != 0);
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

    // cout << "New fifo after applying: ";
    // for (list<Pixel>::iterator it = fifo_->begin(); it != fifo_->end(); it++) {
    //     cout << descriptionforPixel(*it) << " ";;
    // }
    // cout << endl;
    free(fetch_);
    fetch_ = NULL;
}

list<Pixel> PixelList(uint16_t pixels, Palette palette) {
    list<Pixel> ret = list<Pixel>();

    uint8_t row_a = HIGHER8(pixels);
    uint8_t row_b = LOWER8(pixels);
    for (int i = 0; i < 8; i++) {
        Pixel p = Pixel();
        p.palette_ = palette;
        p.two_bit_color_ = ((row_a & 0x1) << 1) | (row_b & 0x1);
        ret.push_front(p);
        row_a >>= 1;
        row_b >>= 1;
    }
    return ret;
}

void PixelFIFO::StartFetch() {
    assert(fetch_ == NULL);
    fetch_ = new Fetch();
    fetch_->cycles_remaining_ = FETCH_CYCLES;
    
    // TODO: Find the right next tile based on where we are.
    int next_tile = x_ + fifo_->size();
    // cout << "Fetching next tile: 0x" << hex << unsigned(next_tile) << endl;
    uint16_t background_tile = ppu_->BackgroundTile(next_tile, y_);
    fetch_->pixels_ = PixelList(background_tile, BackgroundWindowPalette);
    fetch_->strategy_ = AppendFetchStrategy;
}
