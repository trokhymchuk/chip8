#include "Chip8.h"
#include <array>
#include <iostream>
#include <thread>
int main()
{
  std::array<unsigned char, 4096> mem;
  mem[0x200] = 06001 >> 8;
  mem[0x201] = 06001 & 0x00FF;
  Chip8 proc{mem};
  proc.emulate_cycle();

  return 0;
}
