# Ed's Gameboy Emulator

## Features:

✔️ Loads and plays major games

✔️ Pulse, Wave, and Noise Voices play in all games

✔️ Graphics pass all tests.

## Games
✔️ Dr. Mario

✔️ Tetris

✔️ Zelda

✔️ Ducktales

✔️ Pokemón Red

## Tests

✔️ [Blargg](https://github.com/retrio/gb-test-roms) cpu_instrs

🌓 [Blargg](https://github.com/retrio/gb-test-roms) dmg_sound - some tests

✔️ [dmg-acid2](https://github.com/mattcurrie/dmg-acid2) passes

## Instructions
* Install cmake, [rgbds](https://github.com/gbdev/rgbds), and SDL-dev libraries
  * Ubuntu: sudo apt-get install libsdl2-dev cmake rgbds
  * MacOS: brew install sdl cmake rgbds
* mkdir build
* cd build
* cmake -DCMAKE_BUILD_TYPE=[Debug|Release] ..
* Build from IDE: 
  * Open this folder in VSCode
  * MacOS: Choose Clang
  * Click the run button.
