#include "system.hpp"

#include "SDL.h"

#include <cassert>
#include <iostream>

#include "AddressRouter.hpp"
#include "BitCommand.hpp"
#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
#include "PPU.hpp"
#include "Utils.hpp"

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;
uint64_t frame_start_ms;

System::System(string rom_filename) {
    mmu_ = GetMMU(rom_filename);
    ppu_ = new PPU();
    cpu_ = new CPU(mmu_, ppu_);
    InitSDL();
    frame_start_ms = SDL_GetPerformanceCounter();
}

MMU *System::GetMMU(string rom_filename) {
    ROM *boot_rom = new ROM();
    assert(boot_rom->LoadFile("../../boot.gb"));
    ROM *cartridge_rom = new ROM();
    assert(cartridge_rom->LoadFile(rom_filename));

    MMU *mmu = new MMU();
    mmu->SetROMs(boot_rom, cartridge_rom);
    return mmu;
}

void System::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        const char * error = SDL_GetError();
        cout << "Error in SDL_Init: " << error << endl;
        assert(false);
    }
    window_ = SDL_CreateWindow(
        "EDGE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 2,
        SCREEN_HEIGHT * 2,
        0
    );

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    texture_ = SDL_CreateTexture(renderer_,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);

    pixels_ = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    ppu_->SetTexturePixels(pixels_);
}

void System::Advance(int stepped) {
    const int frame_cycles = 70224 / 4;

    ppu_->Advance(stepped);

    cycles_ += stepped;
    if (cycles_ >= frame_cycles) {
        FrameEnded();
        cycles_ = 0;
        uint64_t end_ms = SDL_GetPerformanceCounter();
        cout << "Frame: " << (end_ms - frame_start_ms) / 1000 << endl;
        frame_start_ms = end_ms;
    }
}

void System::FrameEnded() {
    SDL_Event e;
    while(SDL_PollEvent( &e ) != 0) {
        if( e.type == SDL_QUIT ) {
            cout << "SDL_QUIT" << endl;
            exit(0);
        }
    }

    SDL_UpdateTexture(texture_, NULL, pixels_, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
    SDL_RenderPresent(renderer_);
}

void System::Main() {
    // cpu_->SetDebugPrint(true);
    while(true) {
        Advance(cpu_->Step());
    }
}