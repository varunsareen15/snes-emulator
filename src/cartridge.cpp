#include "cartridge.h"
#include <iostream>

using namespace std;

Cartridge::Cartridge(const vector<uint8_t> &file_data) {
  // Check for copier header
  size_t offset = (file_data.size() % 1024 == 512) ? 512 : 0;
  rom = vector<uint8_t>(file_data.begin() + offset, file_data.end());

  size_t header_offset;

  uint16_t lo_check = rom[0x7FDE] | (rom[0x7FDF] << 8);
  uint16_t lo_comp = rom[0x7FDC] | (rom[0x7FDD] << 8);

  uint16_t hi_check = rom[0xFFDE] | (rom[0xFFDF] << 8);
  uint16_t hi_comp = rom[0xFFDC] | (rom[0xFFDD] << 8);

  if ((hi_check + hi_comp) == 0xFFFF) {
    header_offset = 0xFFC0;
  } else if ((lo_check + lo_comp) == 0xFFFF) {
    header_offset = 0x7FC0;
  } else {
    header_offset = 0x0000;
  }

  map_mode = rom[header_offset + 0x15];
  rom_size = rom[header_offset + 0x17];
  ram_size = rom[header_offset + 0x18];
  cartridge_type = rom[header_offset + 0x16];
  checksum = rom[header_offset + 0x1E] | (rom[header_offset + 0x1E + 1] << 8);
  checksum_complement =
      rom[header_offset + 0x1C] | (rom[header_offset + 0x1C + 1] << 8);

  title = string(rom.begin() + header_offset, rom.begin() + header_offset + 21);
}

void Cartridge::print_header() const {
  int rom_size_kb = (rom_size == 0) ? 0 : 1 << rom_size;
  int ram_size_kb = (ram_size == 0) ? 0 : 1 << ram_size;
  string cartridge_type_string = "";
  if (cartridge_type == 0x00) {
    cartridge_type_string = "ROM only";
  } else if (cartridge_type == 0x01) {
    cartridge_type_string = "ROM + RAM";
  } else if (cartridge_type == 0x02) {
    cartridge_type_string = "ROM + RAM + Battery (save games)";
  } else {
    cartridge_type_string = "Error detecting cartridge type";
  }

  cout << "Title: " << title << endl;
  cout << "Map Mode: " << (((map_mode & 0x01) == 0) ? "LoRom" : "HiRom")
       << endl;
  cout << "Rom Size: " << rom_size_kb << " KB" << endl;
  cout << "Ram Size: " << ram_size_kb << " KB" << endl;
  cout << "Cartridge Type: " << cartridge_type_string << endl;
  cout << "Valid Checksum: "
       << ((checksum + checksum_complement == 0xFFFF) ? "Yes" : "No") << endl;
}
