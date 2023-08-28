cmake_minimum_required(VERSION 3.20)
project(Chip8)
include(CTest)
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  # lots of warnings and all warnings as errors
  add_compile_options(
    -std=c++11
    -g
    -O0
    -pedantic-errors
    -fsanitize=address,undefined,leak
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wfloat-equal
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-y2k
    -Wformat=2
    -Wimport
    -Winvalid-pch
    -Wlogical-op
    -Wmissing-declarations
    -Wmissing-field-initializers
    -Wmissing-format-attribute
    -Wmissing-include-dirs
    -Wmissing-noreturn
    -Wpacked
    -Wpointer-arith
    -Wredundant-decls
    -Wstack-protector
    -Wstrict-null-sentinel
    -Wswitch-enum
    -Wundef
    -Wwrite-strings)
endif()
add_link_options(-fsanitize=address,undefined,leak)
set(DEBUG ON)
if(DEBUG)
  add_definitions(-DDEBUG)
endif()
add_subdirectory(src)
add_subdirectory(tests)
add_test(NAME TestProcessor COMMAND Chip8Test)
