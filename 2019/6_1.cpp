#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> Map;

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

int count_parents(const Map& orbits, const std::string &node) {
  int count = 0;
  if (node == "COM")
    return 0;

  std::string current = node;
  do {
    current = orbits.find(current)->second;
    count++;
  } while (current != "COM");

  return count;
}

int main(int argc, char *argv[]) {
  Map orbits;

  if (argc < 2) {
    std::cout << "file name missing" << std::endl;
  }

  orbits["COM"] = "";

  read_map(argv[1], orbits);

  int orbit_sum = 0;
  for (const auto &node : orbits) {
    orbit_sum += count_parents(orbits, node.first);
  }

  std::cout << orbit_sum << std::endl;

  return 0;
}
