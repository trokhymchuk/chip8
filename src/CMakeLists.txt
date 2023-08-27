add_executable(main main.cpp)
target_link_libraries(main PUBLIC Chip8)
add_library(Chip8 OBJECT Chip8.cpp)
