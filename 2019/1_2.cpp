#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  int fuel = 0;
  std::string mass;
  while (std::getline(std::cin, mass)) {
    int module_fuel = std::stoi(mass)/3 - 2;
    int fuel_fuel = module_fuel;
    while (fuel_fuel >= 0) {
      fuel_fuel = fuel_fuel/3 - 2;
      if (fuel_fuel > 0)
        module_fuel += fuel_fuel;
    }
    fuel += module_fuel;
  }
  std::cout << fuel << std::endl;

  return 0;
}

