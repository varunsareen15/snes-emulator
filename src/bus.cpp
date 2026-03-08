#include "bus.h"

using namespace std;

Bus::Bus(const std::vector<uint8_t> &file_data) : rom(file_data) {}

uint8_t Bus::read(uint8_t bank, uint16_t addr) {
  if (bank >= 0x7E && bank <= 0x7F) {
    // WRAM: $7E:0000 - $7F:FFFF (full 128 KB)
    uint32_t offset = ((bank - 0x7E) * 0x10000) + addr;
    return wram[offset];
  }

  if (bank <= 0x3F) {
    if (addr <= 0x1FFF) {
      // WRAM Mirror (first 8 KB)
      return wram[addr];
    }
    if (addr >= 0x8000) {
      // ROM
      uint32_t offset = (bank * 0x10000) + addr;
      return rom[offset];
    }
    // $2100-$7FFF: PPU/IO Registers - stub
    return 0;
  }

  if (0x3F < bank && bank <= 0x7D) {
    // ROM
    uint32_t offset = (bank * 0x10000) + addr;
    return rom[offset];
  }

  if (bank >= 0xC0) {
    // ROM Mirror
    uint32_t offset = ((bank - 0xC0) * 0x10000) + addr;
    return rom[offset];
  }
  return 0; // unmapped
}

void Bus::write(uint8_t bank, uint16_t addr, uint8_t value) {
  if (bank >= 0x7E && bank <= 0x7F) {
    uint32_t offset = ((bank - 0x7E) * 0x10000) + addr;
    wram[offset] = value;
    return;
  }

  if (bank <= 0x3F && addr <= 0x1FFF) {
    wram[addr] = value;
    return;
  }

  // TODO: everything else
}
