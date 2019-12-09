#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#define ADD      1
#define MUL      2
#define IO_IN    3
#define IO_OUT   4
#define J_TRUE   5
#define J_FALSE  6
#define LT       7
#define EQ       8
#define SET_RB   9
#define HALT    99

#define OP_1     0
#define OP_2     1
#define OP_3     2

#define POS_MODE 0
#define IMM_MODE 1
#define REL_MODE 2

#define DEBUG 0

typedef std::vector<int64_t> Memory;
typedef std::deque<int64_t> IO;

class Intcode {
 public:

  enum run_state { kBlocked, kHalted, kOutOfMemory };

  Intcode(const Memory &memory) : _memory(memory) {}

  run_state run(IO &io_in, IO &io_out) {
    while (_pc < _memory.size() && !_halt) {
      Memory params;
      int64_t old_pc = _pc;
      int opcode = decode_instr(_memory, _pc, params);

      switch (opcode) {
      case ADD:
        writemem(params[OP_3], readmem(params[OP_1]) + readmem(params[OP_2]));
        break;
      case MUL:
        writemem(params[OP_3], readmem(params[OP_1]) * readmem(params[OP_2]));
        break;
      case IO_IN:
        if (io_in.size()) {
          writemem(params[OP_1], io_in[0]);
          // std::cout << "reading " << io_in[0] << std::endl;
          io_in.pop_front();
        } else {
          // restore PC
          _pc = old_pc;
          // we're blocked until we get more input
          return kBlocked;
        }
        break;
      case IO_OUT:
        // std::cout << "writing " << _memory[params[OP_1]] << std::endl;
        io_out.push_back(readmem(params[OP_1]));
        break;
      case J_TRUE:
        if (readmem(params[OP_1]) != 0)
          _pc = readmem(params[OP_2]);
        break;
      case J_FALSE:
        if (readmem(params[OP_1]) == 0)
          _pc = readmem(params[OP_2]);
        break;
      case LT:
        if (readmem(params[OP_1]) < readmem(params[OP_2]))
          writemem(params[OP_3], 1);
        else
          writemem(params[OP_3], 0);
        break;
      case EQ:
        if (readmem(params[OP_1]) == readmem(params[OP_2]))
          writemem(params[OP_3], 1);
        else
          writemem(params[OP_3], 0);
        break;
      case SET_RB:
        _rb += readmem(params[OP_1]);
        break;
      case HALT:
        _halt = true;
        break;
      default:
        std::cout << "unknown opcode " << opcode << " at " << old_pc
                  << std::endl;
        break;
      }
    }

    return (_halt ? kHalted : kOutOfMemory);
  }

  int64_t readmem(int64_t address) {
    if (address >= _memory.size()) {
      return 0;
    }
    return _memory[address];
  }

  void writemem(int64_t address, int64_t value) {
    if (address >= _memory.size()) {
      _memory.resize(address + 1);
    }
    _memory[address] = value;
  }

  static void read_memory(const std::string &filename, Memory &memory) {
    std::string line;
    std::ifstream file;
    file.open(filename);

    if (!file.is_open())
      return;

    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string field;
      while (std::getline(ss, field, ',')) {
        memory.push_back(std::stoll(field));
      }
    }

    file.close();
  }

private:
  int decode_instr(const Memory &memory, int64_t &pc, Memory &params) {
    int64_t op = memory[pc];
    int opcode = op % 10;
    op /= 10;
    opcode += (op % 10) * 10;
    op /= 10;
    pc++;

    int num_params = 0;
    switch (opcode) {
    case ADD:
    case MUL:
    case LT:
    case EQ:
      num_params = 3;
      break;
    case IO_IN:
    case IO_OUT:
    case SET_RB:
      num_params = 1;
      break;
    case J_TRUE:
    case J_FALSE:
      num_params = 2;
      break;
    default:
      break;
    }

    for (int i = 0; i < num_params; i++) {
      switch (op % 10) {
      case POS_MODE:
        params.push_back(memory[pc++]);
        break;
      case IMM_MODE:
        params.push_back(pc++);
        break;
      case REL_MODE:
        params.push_back(memory[pc++] + _rb);
      default:
        break;
      }
      op /= 10;
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
  int64_t _pc = 0;
  int64_t _rb = 0;
  bool _halt = false;
};

int main(int argc, char *argv[]) {
  Memory memory;

  if (argc < 2) {
    std::cout << "file name missing" << std::endl;
  }

  Intcode::read_memory(argv[1], memory);
  Intcode machine(memory);

  IO in, out;
  in.push_back(2);
  const auto state = machine.run(in, out);

  std::cout << "machine state " << state << std::endl;

  for (const auto &o : out)
    std::cout << o << std::endl;

  return 0;
}
