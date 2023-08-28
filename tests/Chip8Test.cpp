#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/Chip8.h"
#include "Dumper.h"
#include <array>
#include <cstdio>
#include <doctest/doctest.h>
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
  SUBCASE("2NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x2208 >> 8;
    mem[0x201] = 0x2208 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x208);
    CHECK(dump_sp(proc) == 1);
    CHECK(dump_stack(proc)[0] == 0x202);
  }
  SUBCASE("3NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x3000 >> 8;
    mem[0x201] = 0x3000 & 0x00FF;
    mem[0x204] = 0x3001 >> 8;
    mem[0x205] = 0x3001 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x204);
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x206);
  }
  SUBCASE("4NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x4001 >> 8;
    mem[0x201] = 0x4001 & 0x00FF;
    mem[0x204] = 0x4000 >> 8;
    mem[0x205] = 0x4000 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x204);
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x206);
  }
  SUBCASE("5NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x5010 >> 8;
    mem[0x201] = 0x5010 & 0x00FF;
    mem[0x204] = 0x6001 >> 8;
    mem[0x205] = 0x6001 & 0x00FF;
    mem[0x206] = 0x5010 >> 8;
    mem[0x207] = 0x5010 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x204);
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 0x208);
  }
  SUBCASE("6NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x6001 >> 8;
    mem[0x201] = 0x6001 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 0x01);
  }
  SUBCASE("7NNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x7001 >> 8;
    mem[0x201] = 0x7001 & 0x00FF;
    mem[0x202] = 0x7001 >> 8;
    mem[0x203] = 0x7001 & 0x00FF;

    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 0x01);
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 0x02);
  }
  SUBCASE("8XY0")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x6001 >> 8;
    mem[0x201] = 0x6001 & 0x00FF;
    mem[0x202] = 0x8100 >> 8;
    mem[0x203] = 0x8100 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 1);
  }
  SUBCASE("8XY1")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x60AA >> 8;
    mem[0x201] = 0x60AA & 0x00FF;
    mem[0x202] = 0x6155 >> 8;
    mem[0x203] = 0x6155 & 0x00FF;
    mem[0x204] = 0x8011 >> 8;
    mem[0x205] = 0x8011 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 255);
  }
  SUBCASE("8XY2")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x60AA >> 8;
    mem[0x201] = 0x60AA & 0x00FF;
    mem[0x202] = 0x6157 >> 8;
    mem[0x203] = 0x6157 & 0x00FF;
    mem[0x204] = 0x8012 >> 8;
    mem[0x205] = 0x8012 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 2);
  }
  SUBCASE("8XY3")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x60AA >> 8;
    mem[0x201] = 0x60AA & 0x00FF;
    mem[0x202] = 0x6157 >> 8;
    mem[0x203] = 0x6157 & 0x00FF;
    mem[0x204] = 0x8013 >> 8;
    mem[0x205] = 0x8013 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 253);
  }
  SUBCASE("8XY4")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x6002 >> 8;
    mem[0x201] = 0x6002 & 0x00FF;
    mem[0x202] = 0x6104 >> 8;
    mem[0x203] = 0x6104 & 0x00FF;
    mem[0x204] = 0x8014 >> 8;
    mem[0x205] = 0x8014 & 0x00FF;
    mem[0x206] = 0x61FF >> 8;
    mem[0x207] = 0x61FF & 0x00FF;
    mem[0x208] = 0x8014 >> 8;
    mem[0x209] = 0x8014 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 6);
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 5);
    CHECK(dump_registers(proc)[0xF] == 1);
  }
  SUBCASE("8XY5")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x6004 >> 8;
    mem[0x201] = 0x6004 & 0x00FF;
    mem[0x202] = 0x6102 >> 8;
    mem[0x203] = 0x6102 & 0x00FF;
    mem[0x204] = 0x8015 >> 8;
    mem[0x205] = 0x8015 & 0x00FF;
    mem[0x206] = 0x6000 >> 8;
    mem[0x207] = 0x6000 & 0x00FF;
    mem[0x208] = 0x8015 >> 8;
    mem[0x209] = 0x8015 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 2);
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 254);
    CHECK(dump_registers(proc)[0xF] == 1);
  }
  SUBCASE("8XY6")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x60FF >> 8;
    mem[0x201] = 0x60FF & 0x00FF;
    mem[0x202] = 0x8006 >> 8;
    mem[0x203] = 0x8006 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 127);
    CHECK(dump_registers(proc)[0xF] == 1);
  }
  SUBCASE("8XY7")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x6002 >> 8;
    mem[0x201] = 0x6002 & 0x00FF;
    mem[0x202] = 0x6104 >> 8;
    mem[0x203] = 0x6104 & 0x00FF;
    mem[0x204] = 0x8017 >> 8;
    mem[0x205] = 0x8017 & 0x00FF;
    mem[0x206] = 0x6100 >> 8;
    mem[0x207] = 0x6100 & 0x00FF;
    mem[0x208] = 0x8017 >> 8;
    mem[0x209] = 0x8017 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 2);
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 254);
    CHECK(dump_registers(proc)[0xF] == 1);
  }
  SUBCASE("8XYE")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0x60FF >> 8;
    mem[0x201] = 0x60FF & 0x00FF;
    mem[0x202] = 0x8006 >> 8;
    mem[0x203] = 0x8006 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    proc.emulate_cycle();
    CHECK(dump_registers(proc)[0] == 127);
    CHECK(dump_registers(proc)[0xF] == 1);
  }
  SUBCASE("9XY0")
  {
    SUBCASE("V[X] == V[Y]")
    {
      std::array<unsigned char, 4096> mem;
      mem[0x200] = 0x9000 >> 8;
      mem[0x201] = 0x900 & 0x00FF;
      Chip8 proc{mem};
      proc.emulate_cycle();
      CHECK(dump_pc(proc) == 0x202);
    }
    SUBCASE("V[X] != V[Y]")
    {
      std::array<unsigned char, 4096> mem;
      mem[0x200] = 0x6001 >> 8;
      mem[0x201] = 0x6001 & 0x00FF;
      mem[0x202] = 0x9010 >> 8;
      mem[0x203] = 0x9010 & 0x00FF;
      Chip8 proc{mem};
      proc.emulate_cycle();
      proc.emulate_cycle();
      CHECK(dump_registers(proc)[0] == 1);
      CHECK(dump_pc(proc) == 0x206);
    }
  }
  SUBCASE("ANNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0xA001 >> 8;
    mem[0x201] = 0xA001 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_index_reg(proc) == 1);
  }
  SUBCASE("BNNN")
  {
    std::array<unsigned char, 4096> mem;
    mem[0x200] = 0xB001 >> 8;
    mem[0x201] = 0xB001 & 0x00FF;
    Chip8 proc{mem};
    proc.emulate_cycle();
    CHECK(dump_pc(proc) == 1);
  }
}
