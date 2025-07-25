# Ed's Gameboy Emulator

## Features:

✔️ Runs on macOS and iOS

✔️ Loads and plays games

✔️ Time Travel with Emulator Save/Restore

✔️ Pulse, Wave, and Noise Sound Voices

✔️ Largely cycle accurate emulation

## iOS Emulator
![Zelda Gameplay](/images/screenshot1.png)
![Game Screen](/images/screenshot2.png)

## Games

![Pokemon Red](/images/pokemonred.png)
![Dr. Mario](/images/drmario.png)
![Ducktales](/images/ducktales.png)
![Mario](/images/mario.png)
![Zelda](/images/zelda.png)
![Is that a demo in your pocket?](/images/pocket.png)

## Tests

✔️ [Blargg](https://github.com/retrio/gb-test-roms) cpu_instrs

✔️ [dmg-acid2](https://github.com/mattcurrie/dmg-acid2) passes

🌓 [Blargg](https://github.com/retrio/gb-test-roms) dmg_sound - non-timing


## Instructions
* Install cmake, [rgbds](https://github.com/gbdev/rgbds), and SDL-dev libraries
  * Ubuntu: sudo apt-get install libsdl3-dev cmake rgbds
  * MacOS: brew install sdl3 cmake rgbds
* mkdir build
* cd build
* mkdir ROMs # ROMs will go here
* mkdir States # Saved States will go here
* cmake -DCMAKE_BUILD_TYPE=[Debug|Release] ..
* make && ./edge ROMs/rom.gb States
