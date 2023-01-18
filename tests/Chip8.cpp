#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/Chip8.h"
#include <array>
#include <cstdio>
#include <doctest/doctest.h>
#include "Dumper.h"
void init_mem(std::array<unsigned char, 4096> &mem, char const *const prog_fname)
{
  FILE *prog_file = fopen(prog_fname, "rb");
  int ch;
  for (size_t i{}; i != 4096 && (ch = fgetc(prog_file)) != EOF; ++i)
    mem[i] = static_cast<unsigned char>(ch);
}

TEST_CASE("Check loading memory")
{
  Chip8 proc_no_prog, proc_2048{"../../tests/2048_bytes"}, proc_4096{"../../tests/4096_bytes"},
      proc_5000{"../../tests/5000_bytes"};
  std::array<unsigned char, 4096> mem_no_prog{}, mem_2048, mem_4096, mem_5000;
  init_mem(mem_2048, "../../tests/2048_bytes");
  init_mem(mem_4096, "../../tests/4096_bytes");
  init_mem(mem_5000, "../../tests/5000_bytes");
  CHECK(dump_mem(proc_no_prog) == mem_no_prog);
  CHECK(dump_mem(proc_2048) == mem_2048);
  CHECK(dump_mem(proc_4096) == mem_4096);
  CHECK(dump_mem(proc_5000) == mem_5000);
}

TEST_CASE("Opcodes")
{
  SUBCASE("00E0") { ; }
  SUBCASE("00EE")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x2208 >> 8;
    mem[0x201] = 0x2208 & 0x00FF;
    mem[0x208] = 0x00EE >> 8;
    mem[0x209] = 0x00EE & 0x00FF;
    mem[0x202] = 0x00EE >> 8;
    mem[0x203] = 0x00EE & 0x00FF;
    Chip8 proc{mem};
    CHECK(dump_stack(proc)[0] == 0);
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x208);
    CHECK(dump_sp(proc) == 1);
    CHECK(dump_stack(proc)[0] == 0x202);
    proc.emulate_cycle();
    CHECK(dump_sp(proc) == 0);
    CHECK_THROWS(proc.emulate_cycle());
  }
SUBCASE("1NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x1208 >> 8;
    mem[0x201] = 0x1208 & 0x00FF;
    mem[0x208] = 0x1400 >> 8;
    mem[0x209] = 0x1400 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x208);
    CHECK(dump_sp(proc) == 0);
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x400);
  }
}
