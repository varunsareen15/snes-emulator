#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <string>
#include <vector>

class Cartridge {
public:
  // Constructor - takes raw file bytes, does all the parsing
  Cartridge(const std::vector<uint8_t> &file_data);

  // Print parsed header
  void print_header() const;

private:
  std::vector<uint8_t> rom; // ROM data (after stripping copier header)

  // header fields
  std::string title;
  uint8_t map_mode = 0;
  uint8_t rom_size = 0;
  uint8_t ram_size = 0;
  uint8_t cartridge_type = 0;
  uint16_t checksum = 0;
  uint16_t checksum_complement = 0;
};

#endif // CARTRIDGE_H
