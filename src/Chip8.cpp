#include "Chip8.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
Chip8::Chip8(std::string const &program) : Chip8()
{
  load_font();
  load_program(program);
}
/*
 * Check if reading exceded the memory.
 */
inline static bool exceded_memory(unsigned char)
{
  unsigned short counter{Chip8::PROGRAM_MEMORY_START};
  return ++counter <= Chip8::MEMORY_SIZE;
}
void Chip8::load_program(const std::string &program)
{
  std::ifstream prg{program, std::ifstream::binary};
  prg >> std::noskipws;
  std::istream_iterator<unsigned char> opcode_stream{prg}, opcode_stream_eof;
  std::copy_if(opcode_stream, opcode_stream_eof, memory.begin(), exceded_memory);
  prg.close();
}
void Chip8::emulate_cycle()
{
  proc_timers();
  opcode = static_cast<unsigned short>(memory.at(pc) << 8 | memory.at(pc + 1)); // not an UB! (integral promotion)
  pc += 2;
  switch (opcode & 0xF000) {
  case 0x0000:
    switch (opcode) {
    case 0x00E0:
      disp_clear();
      break;
    case 0x00EE:
      subr_ret();
      break;
    default:
      call();
      break;
    }
    break;
  case 0x1000:
    goto_addr();
    break;
  case 0x2000:
    call_subr();
    break;
  case 0x3000:
    skip_addr_equals();
    break;
  case 0x4000:
    skip_addr_not_equals();
    break;
  case 0x5000:
    skip_reg_equals();
    break;
  case 0x6000:
    set_reg();
    break;
  case 0x7000:
    add_reg();
    break;
  case 0x8000:
    switch (opcode & 0x000F) {
    case 0:
      assign_reg();
      break;
    case 1:
      bitor_reg();
      break;
    case 2:
      bitand_reg();
      break;
    case 3:
      bitxor_reg();
      break;
    case 4:
      sum_reg();
      break;
    case 5:
      substr_reg();
      break;
    case 6:
      rsh_reg();
      break;
    case 7:
      delta_reg();
      break;
    case 0xE:
      lsh_reg();
      break;
    default:
      throw std::runtime_error{"Incorrect opcode " + std::to_string(opcode)};
      break;
    }
    break;
  case 0x9000:
    skip_reg_not_equal();
    break;
  case 0xA000:
    set_index();
    break;
  case 0xB000:
    jmp_r0_nnn();
    break;
  default:
    throw std::runtime_error{"Incorrect opcode " + std::to_string(opcode)};
    break;
  }
}

void Chip8::proc_timers()
{
  static std::chrono::milliseconds old_time{
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())};
  std::chrono::milliseconds time_diff{
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) -
      old_time};
  if (time_diff >= std::chrono::milliseconds{1000 / FREQ}) {
    old_time += time_diff;
    if (delay_timer)
      --delay_timer;
 if (sound_timer == 1)
      ; // TODO: BEEP
    if (sound_timer)
      --sound_timer;
  }
}
