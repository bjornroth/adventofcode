#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define ADD 1
#define MUL 2
#define IO_IN  3
#define IO_OUT 4
#define HALT 99

#define OP_1 0
#define OP_2 1
#define OP_3 2

#define DEBUG 0

int decode_instr(const std::vector<int> &memory, int &pc, std::vector<int> &params) {
  int op = memory[pc];
  int opcode = op % 10;
  op /= 10;
  opcode += (op % 10) * 10;
  op /= 10;
  pc++;

  int num_maybe_immediate = 0;
  int num_indirect = 0;
  switch (opcode) {
  case ADD:
  case MUL:
    num_maybe_immediate = 2;
    num_indirect = 1;
    break;
  case IO_IN:
    num_indirect = 1;
    break;
  case IO_OUT:
    num_maybe_immediate = 1;
    break;
  default:
    break;
  }

  for (int i = 0; i < num_maybe_immediate; i++) {
    switch (op % 10) {
    case 0:
      params.push_back(memory[pc++]);
      break;
    case 1:
      params.push_back(pc++);
      break;
    default:
      break;
    }
    op /= 10;
  }

  for (int i = 0; i < num_indirect; i++) {
    params.push_back(memory[pc++]);
  }

  return opcode;
}

void read_memory(const std::string &filename, std::vector<int> &memory) {
  std::string line;
  std::ifstream file;
  file.open(filename);

  if (!file.is_open())
    return;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
      memory.push_back(std::stoi(field));
    }
  }

  file.close();
}

void dump_memory(const std::vector<int> &memory) {
  for (int i = 0; i < memory.size(); i++) {
    std::cout << memory[i] << ",";
  }
  std::cout << std::endl;
}

int intcode_run(std::vector<int> &memory) {
  int pc = 0;
  bool halt = false;
  while (pc < memory.size() && !halt) {
    std::vector<int> params;
    int old_pc = pc;
    int opcode = decode_instr(memory, pc, params);
#if DEBUG
    std::cout << "pc " << old_pc << " op " << opcode;
#endif
    switch (opcode) {
    case ADD:
#if DEBUG
      std::cout << " params " << params[OP_1] << " " << params[OP_2] << " " << params[OP_3];
#endif
      memory[params[OP_3]] = memory[params[OP_1]] + memory[params[OP_2]];
      break;
    case MUL:
#if DEBUG
      std::cout << " params " << params[OP_1] << " " << params[OP_2] << " " << params[OP_3];
#endif
      memory[params[OP_3]] = memory[params[OP_1]] * memory[params[OP_2]];
      break;
    case IO_IN: {
#if DEBUG
      std::cout << " params " << params[OP_1];
#endif
      std::string input;
      std::getline(std::cin, input);
      memory[params[OP_1]] = std::stoi(input);
      break;
    }
    case IO_OUT: {
#if DEBUG
      std::cout << " params " << params[OP_1];
#endif
      std::cout << memory[params[OP_1]] << std::endl;
      break;
    }
    case HALT:
      halt = true;
      break;
    default:
      std::cout << "unknown opcode " << opcode << " at " << old_pc << std::endl;
      break;
    }
#if DEBUG
    std::cout << std::endl;
#endif
  }
  return pc;
}

int main(int argc, char *argv[]) {
  std::vector<int> memory;

  if (argc < 2) {
    std::cout << "file name missing" << std::endl;
  }

  read_memory(argv[1], memory);
  intcode_run(memory);
#if DEBUG
  dump_memory(memory);
#endif

  return 0;
}
