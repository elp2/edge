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

int main(int argc, char* argv[]) {
    const int SCREEN_WIDTH = 160;
    const int SCREEN_HEIGHT = 144;

    ROM *bootROM = new ROM();
    assert(bootROM->LoadFile("../../boot.gb"));
    ROM *cartridgeROM = new ROM();
    assert(cartridgeROM->LoadFile("../../gb-test-roms/cpu_instrs/cpu_instrs.gb"));

    MMU *mmu = new MMU();
    mmu->SetROMs(bootROM, cartridgeROM);
    PPU *ppu = new PPU();

    CPU *cpu = new CPU(mmu, ppu);
   int status = SDL_Init(SDL_INIT_VIDEO);
    if (status == 0) {
        cout << "OK!";
    } else {
        const char * error = SDL_GetError();
        cout << "Error: " << error << endl;
    }

    SDL_Window *window = SDL_CreateWindow(
        "EDGE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 2,
        SCREEN_HEIGHT * 2,
        0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    cpu->JumpAddress(0x00);

    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    ppu->SetTexturePixels(pixels);

    const int frame_cycles = 70224 / 4;
    int cycles = 0;
    int x = 0;
    SDL_Event e;
    while (true) {
        if (cpu->Get16Bit(Register_PC) == 0x100) {
            cpu->SetDebugPrint(true);
            cout << "CPU Cycles to 0x100: 0x" << hex << unsigned(cpu->Cycles()) << endl;
        }
        cpu->Step();

        cycles += 10;
        if (cycles >= frame_cycles) {
            while(SDL_PollEvent( &e ) != 0) {
                if( e.type == SDL_QUIT ) {
                    cout << "SDL_QuIT" << endl;
                    return 0;
                }
            }

            cycles = 0;
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    cout << "Finished at PC: 0x" << hex << unsigned(cpu->Get16Bit(Register_PC)) << endl;

	return 1;
}