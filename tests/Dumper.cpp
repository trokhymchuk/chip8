#include "Dumper.h"
#include "../src/Chip8.h"
#include <array>

unsigned short dump_pc(const Chip8 &proc) { return proc.pc; }
std::array<unsigned char, 4096> const &dump_mem(const Chip8 &proc) { return proc.memory; };
std::array<unsigned char, 16> const &dump_registers(const Chip8 &proc) { return proc.V; }
unsigned short dump_sp(Chip8 const &proc) { return proc.sp; }
std::array<unsigned short, 16> const &dump_stack(const Chip8 &proc) { return proc.stack; }
