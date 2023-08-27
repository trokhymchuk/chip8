#ifndef DUMPER_H
#define DUMPER_H
#include <array>

class Chip8;
unsigned short dump_pc(const Chip8 &proc);
std::array<unsigned char, 4096> const &dump_mem(const Chip8 &proc);
std::array<unsigned char, 16> const &dump_registers(const Chip8 &proc);
unsigned short dump_sp(Chip8 const &proc);
std::array<unsigned short, 16> const &dump_stack(const Chip8 &proc);

#endif // #ifndef DUMPER_H
