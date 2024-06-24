# Chip-8 Emulator/Interpreter

This is a Chip-8 emulator written in C using SDL2 for graphics and input handling. (hardware quirks unimplemented). It can display most ch8 files, but input performance isn't stable. Works on all test ROMS

## Features

- Emulates the Chip-8 system, capable of running Chip-8 ROMs.
- Provides a simple graphical interface using SDL2 to render graphics.
- Supports keyboard input for Chip-8 keypad emulation.
- Implements sound and delay timers as per Chip-8 specifications.

## Requirements

- SDL2 library must be installed. You can download it from [SDL's official website](https://www.libsdl.org/download-2.0.php) or install it using your system's package manager.

## How to Use

1. **Compile**: Compile the emulator using the makefile: 
   ```bash
   make
2. **Run**: run this command
   ```bash
    ./chip8 <path_to_ROM_file>
