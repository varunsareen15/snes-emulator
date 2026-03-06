# snes-emu

A Super Nintendo emulator written in C++ from scratch, built as a learning project.

## About

This is an educational SNES emulator built to understand low-level hardware emulation — CPU instruction decoding, memory mapping, and eventually PPU rendering and audio. The goal is depth of understanding over feature completeness.

## Build

Requires a C++17-compatible compiler.

```bash
make
./snes <rom_file>
```

To clean build artifacts:

```bash
make clean
```

## Current Status

- **ROM loading** — parses SNES cartridge headers (LoROM/HiROM detection, title, checksum, mapper info)
- **CPU** — 65C816 core in progress; handles emulation/native mode switching, a growing set of opcodes (CLC, XCE, JML, JSL, LDA/LDX/LDY immediate, REP/SEP, TXS, TCD, and others)

## Dependencies

- C++17 compiler (GCC or Clang)

## License
MIT
