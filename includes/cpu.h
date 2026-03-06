#ifndef CPU_H
#define CPU_H

// #include <array>
#include <cstdint>
#include <vector>

class CPU {
public:
  CPU(const std::vector<uint8_t> &rom);
  void step();

  // helpers
  uint8_t read(uint16_t addr);
  // void write(uint16_t addr, uint8_t value);
  bool is_m_flag_set();
  bool is_x_flag_set();

private:
  const std::vector<uint8_t> &rom;
  uint16_t reg_a; // A: Accumulator - math and logic (8 or 16-bit)

  // X, Y: Index registers - looping offsets (8 or 16-bit)
  uint16_t reg_x;
  uint16_t reg_y;

  uint16_t reg_sp; // SP: Stack pointer (16-bit)
  uint16_t reg_pc; // PC: Program counter - current instruction (16-bit)
  uint8_t reg_pbr; // PBR: Program bank register - top 8 bits of instruction
                   // address (8-bit)
  uint8_t reg_dbr; // DBR: Data bank register - default bamnk for data access
                   // (8-bit)
  uint16_t reg_d;  // D: Direct page register - like zero page, but relocatable
                   // (16-bit)
  uint8_t reg_p;   // P: Status flags - N, V, M, X, D, I, Z, C (8-bit)

  bool emulation_mode = true; // CPU starts in emulation mode
  // std::array<uint8_t, 0x20000> wram = {}; // 128 KB work RAM
};

#endif // CPU_H
