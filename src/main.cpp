#include "cartridge.h"
#include <cstdint>
#include <fstream>
#include <iostream>
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
  return 0;
}
