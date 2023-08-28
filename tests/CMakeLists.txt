add_executable(Chip8Test Chip8Test.cpp Dumper.cpp)
target_link_libraries(Chip8Test PUBLIC Chip8)
add_test(NAME TestProcessor COMMAND Chip8Test)
