#include <iostream>
#include <thread>

int main() {
  while (true) {
    std::cout << "test\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return 0;
}
