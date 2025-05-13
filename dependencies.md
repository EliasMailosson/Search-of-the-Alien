# Dependencies
The project requires the following dependencies to be installed on your system:

### Common
- C/C++ Compiler:
    - macOS: clang
    - Windows: gcc (MinGW/MSYS)
    - Linux: clang
- make

### SDL2 Libraries:
- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_mixer
- SDL2_net

## macOS:
Install libraries via Homebrew: 
```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net
```

## Windows:
- Install MinGW/MSYS and include SDL2 development libraries.
- SDL2 headers and binaries should be in C:/msys64/mingw64/include/SDL2 and C:/msys64/mingw64/lib.

## Linux:
Install SDL2 via your package manager:
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev 
```