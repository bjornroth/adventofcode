#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  int fuel = 0;
  std::string mass;
  while (std::getline(std::cin, mass)) {
    fuel += std::stoi(mass)/3 - 2;
  }
  std::cout << fuel << std::endl;

  return 0;
}

