#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> Map;
typedef std::unordered_map<std::string, int> Path;

void read_map(const std::string &filename, Map &orbits) {
  std::string line;
  std::ifstream file;
  file.open(filename);

  if (!file.is_open())
    return;

  while (std::getline(file, line)) {
    int sep = line.find_first_of(")");
    std::string parent = line.substr(0, sep);
    std::string child = line.substr(sep + 1);
    orbits[child] = parent;
  }
}

Path get_parent_list(const Map& orbits, const std::string &node) {
  Path path;
  int count = 0;

  std::string current = node;
  do {
    current = orbits.find(current)->second;
    path[current] = count;
    count++;
  } while (current != "COM");

  return path;
}

int main(int argc, char *argv[]) {
  Map orbits;

  if (argc < 2) {
    std::cout << "file name missing" << std::endl;
  }

  orbits["COM"] = "";

  read_map(argv[1], orbits);
  Path you = get_parent_list(orbits, "YOU");
  Path santa = get_parent_list(orbits, "SAN");

  int min_sum = INT_MAX;
  for (const auto &y : you) {
    auto s = santa.find(y.first);
    if (s != santa.end()) {
      if (y.second + s->second < min_sum)
        min_sum = y.second + s->second;
    }
  }

  std::cout << min_sum << std::endl;

  return 0;
}
