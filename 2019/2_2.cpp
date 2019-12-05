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

int intcode_run(std::vector<int> &memory) {
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
  return pc;
}

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

  for (int noun = 0; noun < 99; noun++) {
    for (int verb = 0; verb < 99; verb++) {
      std::vector<int> memory_copy = memory;
      memory_copy[1] = noun;
      memory_copy[2] = verb;
      intcode_run(memory_copy);
      if (memory_copy[0] == 19690720) {
        std::cout << 100 * noun + verb << std::endl;
      }
    }
  }
  return 0;
}
