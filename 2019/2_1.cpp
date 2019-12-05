#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define ADD 1
#define MUL 2
#define HALT 99

#define OP_1_OFFS 1
#define OP_2_OFFS 2
#define WRITE_ADDR_OFFS 3
#define INSTRUCTION_SIZE 4

int main(int argc, char *argv[]) {
  std::vector<int> memory;
  std::string line;

  while (std::getline(std::cin, line)) {
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
      memory.push_back(std::stoi(field));
    }
  }

  memory[1] = 12;
  memory[2] = 2;

  int pc = 0;
  bool halt = false;
  while (pc < memory.size() && !halt) {
    switch (memory[pc]) {
    case ADD:
      memory[memory[pc+WRITE_ADDR_OFFS]] = memory[memory[pc+OP_1_OFFS]] + memory[memory[pc+OP_2_OFFS]];
      break;
    case MUL:
      memory[memory[pc+WRITE_ADDR_OFFS]] = memory[memory[pc+OP_1_OFFS]] * memory[memory[pc+OP_2_OFFS]];
      break;
    case HALT:
      halt = true;
      break;
    default:
      std::cout << "unknown opcode " << memory[pc] << " at " << pc << std::endl;
      break;
    }
    pc += INSTRUCTION_SIZE;
  }

  for (int i = 0; i < memory.size(); i++) {
    std::cout << memory[i] << ",";
  }
  std::cout << std::endl;

  return 0;
}
