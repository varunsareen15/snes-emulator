#include "cpu.h"
#include <iostream>

using namespace std;

CPU::CPU(const vector<uint8_t> &file_data)
    : rom(file_data), reg_a(0), reg_x(0), reg_y(0), reg_sp(0), reg_pbr(0),
      reg_dbr(0), reg_d(0), reg_p(0) {
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

  reg_pc = rom[header_offset + 0x3C] | (rom[header_offset + 0x3D] << 8);
  cout << "Reset vector: 0x" << hex << reg_pc << endl;
}

uint8_t CPU::read(uint16_t addr) {
  // temp HiRom mapping - will be replaced by bus
  uint8_t bank = reg_pbr & 0x3F;
  uint32_t offset = (bank * 0x10000) + addr;
  return rom[offset];
}

void CPU::write(uint16_t addr, uint8_t value) {
  // temp will be replaced by bus
  wram[addr] = value;
}

bool CPU::is_m_flag_set() { return reg_p & 0x20; }
bool CPU::is_x_flag_set() { return reg_p & 0x10; }

StepResult CPU::step() {
  uint8_t opcode = read(reg_pc);
  reg_pc++;
  OpStatus status = OpStatus::Unknown;

  switch (opcode) {
  case 0x18: // CLC - Clear Carry
    // clear bit 0 of reg_p
    reg_p &= 0xFE;
    cout << "0x18 step successful - CLC" << endl;
    status = OpStatus::Implemented;
    break;
  case 0xFB: { // XCE - Exchange Carry & Emulation
    bool old_carry = reg_p & 0x01;
    bool old_emu = emulation_mode;
    emulation_mode = old_carry;
    if (old_emu) {
      reg_p |= 0x01; // set carry
    } else {
      reg_p &= 0xFE; // clear carry
    }
    // When entering native mode, M and X flags should be set
    if (!emulation_mode && old_emu) {
      reg_p |= 0x30; // set M and X flags (8-bit registers)
    }
    cout << "0xFB step successful - XCE" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x5C: {                 // JML - Jump Long
    uint8_t ll = read(reg_pc); // low byte of new PC
    reg_pc++;
    uint8_t mm = read(reg_pc); // high byte of new PC
    reg_pc++;
    uint8_t hh = read(reg_pc); // no increment - we're replacing PC
    reg_pc = ll | (mm << 8);
    reg_pbr = hh;
    cout << "JML to $" << hex << (int)reg_pbr << ":" << reg_pc << endl;
    cout << "0x5C step successful - JML" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x9C: {   // STZ Absolute - skip for now
    reg_pc += 2; // skip the 2-byte address
    cout << "0x9C step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  }
  case 0x64: {   // STZ Direct Page - skip for now
    reg_pc += 1; // skip the 1-byte operand
    cout << "0x64 step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  }
  case 0xA2: { // LDX - Load X Immediate
    if (is_x_flag_set()) {
      reg_x = read(reg_pc);
      reg_pc++;
    } else {
      uint8_t lo = read(reg_pc);
      reg_pc++;
      uint8_t hi = read(reg_pc);
      reg_pc++;
      reg_x = lo | (hi << 8);
    }
    cout << "0xA2 step successful - LDX" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0xA0: { // LDY - Load Y Immediate
    if (is_x_flag_set()) {
      reg_y = read(reg_pc);
      reg_pc++;
    } else {
      uint8_t lo = read(reg_pc);
      reg_pc++;
      uint8_t hi = read(reg_pc);
      reg_pc++;
      reg_y = lo | (hi << 8);
    }
    cout << "0xA0 step successful - LDY" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0xC2: { // REP - Reset Processor status bits
    uint8_t operand = read(reg_pc);
    reg_pc++;
    reg_p &= ~operand;
    cout << "0xC2 step successful - REP" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0xA9: { // LDA - Load Accumulator Immediate
    if (is_m_flag_set()) {
      reg_a = read(reg_pc);
      reg_pc++;
    } else {
      uint8_t lo = read(reg_pc);
      reg_pc++;
      uint8_t hi = read(reg_pc);
      reg_pc++;
      reg_a = lo | (hi << 8);
    }
    cout << "0xA9 step successful - LDA" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x9A: // TXS - Transfer X to Stack Pointer
    reg_sp = reg_x;
    cout << "0x9A step successful - TXS" << endl;
    status = OpStatus::Implemented;
    break;
  case 0x5B: // TCD - Trasnfer Accumulator to Direct Page Register
    reg_d = reg_a;
    cout << "0x5B step successful - TCD" << endl;
    status = OpStatus::Implemented;
    break;
  case 0xE2: { // SEP - Set Processor Status Bits
    uint8_t operand = read(reg_pc);
    reg_pc++;
    reg_p |= operand;
    cout << "0xE2 step successful - SEP" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x01: // ORA (dp,X) - skip for now
    reg_pc += 1;
    cout << "0x01 step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  case 0x54: // MVN - skip for now
    reg_pc += 2;
    cout << "0x54 step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  case 0x30: // BMI - Branch if Negative flag set - skip for now
    reg_pc += 1;
    cout << "0x30 step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  case 0x8D: // STA Absolute - Store A to Memory - skip for now
    reg_pc += 2;
    cout << "0x8D step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  case 0x22: { // JSL - Jump to Subroutine Long
    // push program bank
    write(reg_sp, reg_pbr);
    reg_sp--;
    // push return address (PC + 2, since operand is 3 bytes, but we push PC-1
    // of the last byte
    uint8_t ll = read(reg_pc);
    reg_pc++;
    uint8_t mm = read(reg_pc);
    reg_pc++;
    uint8_t hh = read(reg_pc);
    // push current PC (points at last byte of instruction)
    write(reg_sp, (reg_pc >> 8) & 0xFF); // high byte
    reg_sp--;
    write(reg_sp, reg_pc & 0xFF); // low byte
    reg_sp--;
    // Jump
    reg_pc = ll | (mm << 8);
    reg_pbr = hh;
    cout << "0x22 step successful - JSL" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x0B: { // PHD - Push Direct Page to Stack
    // push high byte
    write(reg_sp, (reg_d >> 8) & 0xFF);
    reg_sp--;
    // push low byte
    write(reg_sp, reg_d & 0xFF);
    reg_sp--;
    cout << "0x0B step successful - PHD" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x8B: { // PHB - Push Data Bank to Stack
    // push data bank
    write(reg_sp, reg_dbr);
    reg_sp--;
    cout << "0x8B step successful - PHB" << endl;
    status = OpStatus::Implemented;
    break;
  }
  case 0x8E: // STX Absolute - skip for now
    reg_pc += 2;
    cout << "0x8E step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  case 0x58: // CLI - Clear Interrupt Disable Flag
    reg_p &= ~0x04;
    cout << "0x58 step successful - CLI" << endl;
    status = OpStatus::Implemented;
    break;
  case 0xAE: // LDX Absolute - skip for now
    reg_pc += 2;
    cout << "0xAE step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  case 0xCA: // DEX - Decrement X by 1
    reg_x--;
    if (is_x_flag_set()) {
      reg_x &= 0xFF;
    }
    cout << "0xCA step successful - DEX" << endl;
    status = OpStatus::Implemented;
    break;
  case 0xF0: // BEQ - branch if Zero flag set - skip for now
    reg_pc += 1;
    cout << "0xF0 step successful (skipped)" << endl;
    status = OpStatus::Skipped;
    break;
  default:
    cout << "Unknown opcode: 0x" << hex << (int)opcode << endl;
    status = OpStatus::Unknown;
    break;
  }

  return {opcode, status};
}
