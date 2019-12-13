#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

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

typedef std::vector<int64_t> Memory;
typedef std::vector<std::pair<int64_t, int>> Params;
typedef std::deque<int64_t> IO;
typedef std::map<int64_t, bool> Breakpoints;

class Intcode {
 public:
   enum run_state { kReady, kBlocked, kBreakpoint, kHalted, kOutOfMemory };

   std::map<int, std::string> mnemonics = {
       {ADD, "ADD"},       {MUL, "MUL"},       {IO_IN, "IO_IN"},
       {IO_OUT, "IO_OUT"}, {J_TRUE, "J_TRUE"}, {J_FALSE, "J_FALSE"},
       {LT, "LT"},         {EQ, "EQ"},         {SET_RB, "SET_RB"},
       {HALT, "HALT"}};
   std::map<run_state, std::string> states = {{kReady, "Ready"},
                                              {kBlocked, "IO"},
                                              {kBreakpoint, "Break"},
                                              {kHalted, "Halted"},
                                              {kOutOfMemory, "OOM"}};

   Intcode(const Memory &memory) : _memory(memory) {}

   run_state execute_instr(IO &io_in, IO &io_out) {
     if (_pc >= _memory.size()) {
       return kOutOfMemory;
     }

     int64_t old_pc = _pc;
     Params params;
     int opcode = decode_instr(_pc, params);

     switch (opcode) {
     case ADD:
       writemem(params[OP_3].first,
                readmem(params[OP_1].first) + readmem(params[OP_2].first));
       break;
     case MUL:
       writemem(params[OP_3].first,
                readmem(params[OP_1].first) * readmem(params[OP_2].first));
       break;
     case IO_IN:
       if (io_in.size()) {
         writemem(params[OP_1].first, io_in[0]);
         io_in.pop_front();
       } else {
         // restore PC
         _pc = old_pc;
         // we're blocked until we get more input
         return kBlocked;
       }
       break;
     case IO_OUT:
       io_out.push_back(readmem(params[OP_1].first));
       break;
     case J_TRUE:
       if (readmem(params[OP_1].first) != 0)
         _pc = readmem(params[OP_2].first);
       break;
     case J_FALSE:
       if (readmem(params[OP_1].first) == 0)
         _pc = readmem(params[OP_2].first);
       break;
     case LT:
       if (readmem(params[OP_1].first) < readmem(params[OP_2].first))
         writemem(params[OP_3].first, 1);
       else
         writemem(params[OP_3].first, 0);
       break;
     case EQ:
       if (readmem(params[OP_1].first) == readmem(params[OP_2].first))
         writemem(params[OP_3].first, 1);
       else
         writemem(params[OP_3].first, 0);
       break;
     case SET_RB:
       _rb += readmem(params[OP_1].first);
       break;
     case HALT:
       _halt = true;
       break;
     default:
       std::cout << "unknown opcode " << opcode << " at " << old_pc
                 << std::endl;
       break;
     }

     if (_breakpoints.find(_pc) != _breakpoints.end()) {
       return kBreakpoint;
     }

     return (_halt ? kHalted : kReady);
  }

  run_state run(IO &io_in, IO &io_out) {
    run_state state;
    do {
      state = execute_instr(io_in, io_out);
    } while (state == kReady);

    return state;
  }

  void run_debugger() {
    bool quit = false;
    run_state state = kReady;
    IO io_in, io_out;
    while (!quit) {
      std::string line;
      auto pc = _pc;

      for (const auto &out : io_out) {
        std::cout << "output: " << out << std::endl;
      }
      if (state != kReady) {
        std::cout << print_instr(pc) << std::endl;
      }
      std::cout << states[state];
      std::cout << "> ";
      std::getline(std::cin, line);
      if (state == kBlocked) {
        io_in.push_back(std::stoll(line));
        state = kReady;
      } else if (line.substr(0, 1) == "n") {
	state = execute_instr(io_in, io_out);
      } else if (line.substr(0, 1) == "c") {
	state = run(io_in, io_out);
      } else if (line.substr(0, 1) == "l") {
        pc = _pc;
        for (int i = 0; i < 10; i++) {
          std::cout << print_instr(pc) << std::endl;
        }
      } else if (line.substr(0, 1) == "p") {
        int addr = std::stoll(line.substr(2));
        std::cout << readmem(addr) << std::endl;
      } else if (line.substr(0, 1) == "b") {
        if (line.size() > 2) {
          int addr = std::stoll(line.substr(2));
          if (_breakpoints.find(addr) != _breakpoints.end()) {
            _breakpoints[addr] = false;
            std::cout << "Breakpoint removed" << std::endl;
          } else {
            _breakpoints[addr] = true;
            std::cout << "Breakpoint at " << addr << std::endl;
          }
        } else {
          for (const auto &bp : _breakpoints) {
            if (bp.second) {
              std::cout << bp.first << std::endl;
            }
          }
        }
      } else if (line.substr(0, 1) == "q") {
        quit = true;
      }
    }
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
  int decode_instr(int64_t &pc, Params &params) {
    int64_t op = _memory[pc];
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
        params.push_back(std::make_pair(_memory[pc++], POS_MODE));
        break;
      case IMM_MODE:
        params.push_back(std::make_pair(pc++, IMM_MODE));
        break;
      case REL_MODE:
        params.push_back(std::make_pair(_memory[pc++] + _rb, REL_MODE));
      default:
        break;
      }
      op /= 10;
    }

    return opcode;
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

  std::string print_instr(int64_t &pc) {
    std::string line;
    Params params;
    auto old_pc = pc;
    int opcode = decode_instr(pc, params);
    if (_breakpoints.find(old_pc) != _breakpoints.end()) {
      line += "+ ";
    } else {
      line += "  ";
    }
    line += std::to_string(old_pc) + " " + mnemonics[opcode];
    for (int i = 0; i < params.size(); i++) {
      switch (params[i].second) {
      case POS_MODE:
	line += " [" + std::to_string(_memory[++old_pc]) + "]";
	break;
      case IMM_MODE:
	line += " #" + std::to_string(_memory[++old_pc]);
	break;
      case REL_MODE:
	line += " (" + std::to_string(_memory[++old_pc]) + ")";
	break;
      }
    }
    return line;
  }

  Memory _memory;
  Breakpoints _breakpoints;
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

  Intcode::run_state state = Intcode::kReady;
  IO in, out;
  while (state != Intcode::kHalted) {
    state = machine.run(in, out);
  }

  std::map<std::pair<int, int>, int> grid;

  int max_x = 0;
  int max_y = 0;
  int tiles = 0;
  while (out.size() >= 3) {
    // std::cout << out[0] << "," << out[1] << ": " << out[2] << std::endl;
    max_x = std::max(max_x, (int)out[0]);
    max_y = std::max(max_y, (int)out[1]);
    grid[std::make_pair(out[0], out[1])] = out[2];
    out.pop_front();
    out.pop_front();
    out.pop_front();
  }

  for (int y = 0; y < max_y; y++) {
    for (int x = 0; x < max_x; x++) {
      const auto coord = std::make_pair(x, y);
      if (grid.find(coord) != grid.end()) {
        switch (grid[coord]) {
        case 0:
          std::cout << " ";
          break;
        case 1:
          std::cout << "W";
          break;
        case 2:
          std::cout << "T";
          tiles++;
          break;
        case 3:
          std::cout << "-";
          break;
        case 4:
          std::cout << "o";
          break;
        }
      } else {
        std::cout << "?";
      }
    }
    std::cout << std::endl;
  }

  std::cout << tiles << std::endl;

  return 0;
}
