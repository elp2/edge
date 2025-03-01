# Ed's Gameboy Emulator

## Status:

✔️ Boot Screen Works

✔️ Pulse, Wave, and Noise Voices play in all games

✔️ PPU features excluding tall sprites

### Games
✔️ Dr. Mario

✔️ Tetris

🌓 Zelda

🌓 Ducktales

❌ Pokemón Red

### Tests

✔️ All Blarg CPU tests pass

🌓 Many DMG_SOUND tests pass

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
