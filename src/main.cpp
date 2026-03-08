#include "cartridge.h"
#include "cpu.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: ./snes <rom_file>" << endl;
    return 1;
  }

  ifstream file(argv[1], ios::binary);
  if (!file) {
    cerr << "Failed to open file" << endl;
    return 1;
  }

  vector<uint8_t> data((istreambuf_iterator<char>(file)),
                       istreambuf_iterator<char>());

  Cartridge cart(data);
  cart.print_header();
	Bus bus(data);
  Cpu cpu(bus);

  // Unique Step Counter
  set<uint8_t> implemented, skipped, unknown;
  for (int i = 1; i <= 1024; i++) {
    cout << "Step " << dec << i << ": ";
    auto result = cpu.step();
    switch (result.status) {
    case OpStatus::Implemented:
      implemented.insert(result.opcode);
      break;
    case OpStatus::Skipped:
      skipped.insert(result.opcode);
      break;
    case OpStatus::Unknown:
      unknown.insert(result.opcode);
      break;
    }
  }
  cout << "\nImplemented: " << dec << implemented.size() << "\nSkipped: " << dec
       << skipped.size() << "\nUnknown: " << dec << unknown.size() << endl;
  return 0;
}
