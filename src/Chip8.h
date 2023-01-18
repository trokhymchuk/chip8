#ifndef CHIP_8_H
#define CHIP_8_H
#include "../tests/Dumper.h"
#include <array>
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
  /** 8XY0: assign V[X] to V[Y] */
  void assign_reg();
  /** 8XY1: V[X] |= V[Y] */
  void bitor_reg();
  /** 8XY1: V[X] &= V[Y] */
  void bitand_reg();

  friend unsigned short dump_pc(const Chip8 &proc);
  friend std::array<unsigned char, 4096> const &dump_mem(const Chip8 &proc);
  friend std::array<unsigned char, 16> const &dump_registers(const Chip8 &proc);
  friend unsigned short dump_sp(Chip8 const &proc);
  friend std::array<unsigned short, 16> const &dump_stack(const Chip8 &proc);
};
inline void Chip8::subr_ret()
{
  if (sp == 0)
    throw std::runtime_error{"Returning not from main thread"};
  pc = stack[--sp];
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
  if (V[opcode & 0X0F00] == (opcode & 0x00FF))
    pc += 2;
}
inline void Chip8::skip_addr_not_equals()
{
  if (V[opcode & 0X0F00] != (opcode & 0x00FF))
    pc += 2;
}
inline void Chip8::skip_reg_equals()
{
  if (V[opcode & 0x0F00] == V[opcode & 0x00F0])
    pc += 2;
}
inline void Chip8::set_reg() { V[opcode & 0x0F00] = static_cast<unsigned char>(opcode & 0x00FF); }
inline void Chip8::add_reg() { V[opcode & 0x0F00] += static_cast<unsigned char>(opcode & 0x00FF); }
inline void Chip8::assign_reg() { V[opcode & 0x0F00] = V[opcode & 0x00F0]; }
inline void Chip8::bitor_reg() { V[opcode & 0x0F00] |= V[opcode & 0x00F0]; }
inline void Chip8::bitand_reg() { V[opcode & 0x0F00] &= V[opcode & 0x00F0]; }
#endif // #ifndef CHIP_8_H
