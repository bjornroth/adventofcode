#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#define ADD     1
#define MUL     2
#define IO_IN   3
#define IO_OUT  4
#define J_TRUE  5
#define J_FALSE 6
#define LT      7
#define EQ      8
#define HALT   99

#define OP_1    0
#define OP_2    1
#define OP_3    2

#define DEBUG 0

typedef std::vector<int> Memory;
typedef std::deque<int> IO;

class Intcode {
 public:
  Intcode(const Memory &memory) : _memory(memory) {}

  int run(IO &io_in, IO &io_out) {
    int pc = 0;
    bool halt = false;
    while (pc < _memory.size() && !halt) {
      std::vector<int> params;
      int old_pc = pc;
      int opcode = decode_instr(_memory, pc, params);

      switch (opcode) {
      case ADD:
        _memory[params[OP_3]] = _memory[params[OP_1]] + _memory[params[OP_2]];
        break;
      case MUL:
        _memory[params[OP_3]] = _memory[params[OP_1]] * _memory[params[OP_2]];
        break;
      case IO_IN:
        _memory[params[OP_1]] = io_in[0];
        // std::cout << "reading " << io_in[0] << std::endl;
        io_in.pop_front();
        break;
      case IO_OUT:
        // std::cout << "writing " << _memory[params[OP_1]] << std::endl;
        io_out.push_back(_memory[params[OP_1]]);
        break;
      case J_TRUE:
        if (_memory[params[OP_1]] != 0)
          pc = _memory[params[OP_2]];
        break;
      case J_FALSE:
        if (_memory[params[OP_1]] == 0)
          pc = _memory[params[OP_2]];
        break;
      case LT:
        if (_memory[params[OP_1]] < _memory[params[OP_2]])
          _memory[params[OP_3]] = 1;
        else
          _memory[params[OP_3]] = 0;
        break;
      case EQ:
        if (_memory[params[OP_1]] == _memory[params[OP_2]])
          _memory[params[OP_3]] = 1;
        else
          _memory[params[OP_3]] = 0;
        break;
      case HALT:
        halt = true;
        break;
      default:
        std::cout << "unknown opcode " << opcode << " at " << old_pc
                  << std::endl;
        break;
      }
    }
    return pc;
  }

  static void read_memory(const std::string &filename,
                          std::vector<int> &memory) {
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

private:
  int decode_instr(const std::vector<int> &memory, int &pc,
                   std::vector<int> &params) {
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
    case LT:
    case EQ:
      num_maybe_immediate = 2;
      num_indirect = 1;
      break;
    case IO_IN:
      num_indirect = 1;
      break;
    case IO_OUT:
      num_maybe_immediate = 1;
      break;
    case J_TRUE:
    case J_FALSE:
      num_maybe_immediate = 2;
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

  void dump_memory() {
    for (int i = 0; i < _memory.size(); i++) {
      std::cout << _memory[i] << ",";
    }
    std::cout << std::endl;
  }

  Memory _memory;
};

int run_amp_chain(const Memory &memory, const std::vector<int> &phases) {
  IO in;
  in.push_front(0);

  for (int i = 0; i < phases.size(); i++) {
    Intcode amp(memory);
    in.push_front(phases[i]);
    IO out;
    amp.run(in, out);
    in = out;
  }

  return in[0];
}

int main(int argc, char *argv[]) {
  std::vector<int> memory;

  if (argc < 2) {
    std::cout << "file name missing" << std::endl;
  }

  Intcode::read_memory(argv[1], memory);

  std::vector<int> phases = { 0, 1, 2, 3, 4 };
  int max_signal = 0;

  do {
    max_signal = std::max(max_signal, run_amp_chain(memory, phases));
  }
  while (std::next_permutation(phases.begin(), phases.end()));

  std::cout << max_signal << std::endl;

  return 0;
}
