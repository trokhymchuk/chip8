#ifndef CHIP_8_H
#define CHIP_8_H
#include "../tests/Dumper.h"
#include <array>
#include <ctime>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

class Chip8 {
public:
  Chip8() : opcode{}, memory{}, V{}, I{}, pc{0x200}, screen{}, delay_timer{}, sound_timer{60}, stack{}, sp{}, key{} {}
  Chip8(std::string const &program);
  Chip8(std::array<unsigned char, 4096> mem) : Chip8() { memory.swap(mem); }
  // TODO: implement
  void load_font() {}
  /** \param program path to the program file */
  void load_program(std::string const &program);
  void emulate_cycle();
  /** max memory size */
  static constexpr unsigned short MEMORY_SIZE{4096};
  /** Where to put the program */
  static constexpr unsigned short PROGRAM_MEMORY_START{0x200};
  static constexpr unsigned short FREQ = 60;

private:
  /** process timers and do the things (_should be called every cycle_) */
  void proc_timers();
  /** terminate the program exection */
  void terminate_exection();
  /** the current opcode */
  unsigned short opcode;
  /** memory of the cpu */
  std::array<unsigned char, MEMORY_SIZE> memory;
  /** V0 - V15 registers */
  std::array<unsigned char, 16> V;
  /** Index register */
  unsigned short I;
  /** program counter */
  unsigned short pc;
  /** screen state (gfx) */
  std::array<bool, 2048> screen;
  unsigned char delay_timer;
  unsigned char sound_timer;
  std::array<unsigned short, 16> stack;
  /** stack pointer */
  unsigned short sp;
  std::array<unsigned char, 16> key;
  // OPCODES
  /** TODO: 0NNN: call */
  void call() {}
  /** 00E0: clear the display */
  void disp_clear();
  /** 00EE: return from subroutine */
  void subr_ret();
  /** 1NNN: goto NNN */
  void goto_addr();
  /** 2NNN: call subroutine on NNN */
  void call_subr();
  /** 3XNN: skip next instruction if V[X] equals NN */
  void skip_addr_equals();
  /** 4XNN: skip next instruction if V[X] NOT equals NN */
  void skip_addr_not_equals();
  /** 5XY0: sip if V[X] == V[Y] */
  void skip_reg_equals();
  /** 6XNN: set V[X] to NN */
  void set_reg();
  /** 7XNN: add NN to V[X] */
  void add_reg();
  /** 8XY0: assign V[X] = V[Y] */
  void assign_reg();
  /** 8XY1: V[X] |= V[Y] */
  void bitor_reg();
  /** 8XY2: V[X] &= V[Y] */
  void bitand_reg();
  /** 8XY3 V[X] ^= V[Y] */
  void bitxor_reg();
  /** 8XY4 V[X] += V[Y] */
  void sum_reg();
  /** 8XY5 V[X] -= V[Y] */
  void substr_reg();
  /** 8XY6 V[0xF] = V[X] & 0xf; V[X] >>= 1;  */
  void rsh_reg();
  /** 8XY7 V[X] = V[Y] - V[X]; */
  void delta_reg();
  /** 8XYE V[0xF] = V[X] & 0xf; V[X] <<= 1;  */
  void lsh_reg();
  /** 9XY0: skip if V[X] != V[Y] */
  void skip_reg_not_equal();
  /** ANNN: I = NNN */
  void set_index();
  /** BNNN: jump V0 + NNN */
  void jmp_r0_nnn();
  /** CXNN: V[X] = random_unsigned_char & NN; **/
  void rand_reg();
  friend unsigned short dump_pc(const Chip8 &proc);
  friend std::array<unsigned char, 4096> const &dump_mem(const Chip8 &proc);
  friend std::array<unsigned char, 16> const &dump_registers(const Chip8 &proc);
  friend unsigned short dump_sp(Chip8 const &proc);
  friend std::array<unsigned short, 16> const &dump_stack(const Chip8 &proc);
  friend unsigned short dump_index_reg(const Chip8 &proc);
};
inline void Chip8::subr_ret()
{
  if (sp == 0)
    throw std::runtime_error{"Returning not from main thread"};
  pc = stack.at(--sp);
}
inline void Chip8::disp_clear()
{
  for (bool &pixel : screen)
    pixel = false;
}
inline void Chip8::goto_addr() { pc = opcode & 0x0FFF; }
inline void Chip8::call_subr()
{
  if (sp == stack.size() - 1)
    throw std::runtime_error("Exceeded subroutine depth of 16!");
  stack[sp++] = pc;
  pc = opcode & 0x0FFF;
}
inline void Chip8::skip_addr_equals()
{
  if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
    pc += 2;
}
inline void Chip8::skip_addr_not_equals()
{
  if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    pc += 2;
}
inline void Chip8::skip_reg_equals()
{
  if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
    pc += 2;
}
inline void Chip8::set_reg() { V[(opcode & 0x0F00) >> 8] = static_cast<unsigned char>(opcode & 0x00FF); }
inline void Chip8::add_reg() { V[(opcode & 0x0F00) >> 8] += static_cast<unsigned char>(opcode & 0x00FF); }
inline void Chip8::assign_reg() { V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]; }
inline void Chip8::bitor_reg() { V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4]; }
inline void Chip8::bitand_reg() { V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4]; }
inline void Chip8::bitxor_reg() { V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4]; }
inline void Chip8::sum_reg()
{
  V[0xF] = (V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4]) > 255; // happy debugging (integral promotion)
  V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
}
inline void Chip8::substr_reg()
{
  V[0xF] = V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4];
  V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
}
inline void Chip8::rsh_reg()
{
  V[0xF] = V[(opcode & 0x0F00) >> 8] & 1;
  V[(opcode & 0x0F00) >> 8] >>= 1;
}
inline void Chip8::delta_reg()
{
  V[0xF] = V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4];
  V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
}
inline void Chip8::lsh_reg()
{
  V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x80;
  V[(opcode & 0x0F00) >> 8] >>= 1;
}

inline void Chip8::skip_reg_not_equal()
{
  if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
    pc += 2;
}

inline void Chip8::set_index() { I = opcode & 0x0FFF; }

inline void Chip8::jmp_r0_nnn() { pc = V[0] + opcode & 0x0FFF; }

inline void Chip8::rand_reg()
{
  std::default_random_engine rand;
#ifndef DEBUG
  rand.seed(static_cast<std::default_random_engine::result_type>(std::time(nullptr)));
#endif // #ifndef DEBUG
  std::uniform_int_distribution<unsigned char> distribution;
  V[(opcode & 0x0F00) >> 8] = distribution(rand) & (opcode & 0x00FF);
}
#endif // #ifndef CHIP_8_H
