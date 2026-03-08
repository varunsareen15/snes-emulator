#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>
#include <vector>

class Bus {
public:
  Bus(const std::vector<uint8_t> &rom);
	uint8_t read(uint8_t bank, uint16_t addr);
	void write(uint8_t bank, uint16_t addr, uint8_t value);

private:
  const std::vector<uint8_t> &rom;
  std::array<uint8_t, 0x20000> wram = {};
};
#endif // BUS_H
