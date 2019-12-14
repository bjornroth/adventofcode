#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

typedef std::pair<int, std::string> Amount;
typedef std::map<std::string, std::pair<int, std::vector<Amount>>> Reactions;
typedef std::map<std::string, int> Materials;

void parse_amount(const std::string &str, std::string &type, int &count) {
  auto line = str;
  line.erase(0, line.find_first_not_of(" "));
  const auto pos = line.find(" ");
  count = std::stoi(line.substr(0, pos));
  type = line.substr(pos);
  type.erase(0, type.find_first_not_of(" "));
  type.erase(type.find_last_not_of(" \n\r\t") + 1);
}

int process_reactions(Materials &materials, Reactions &reactions) {
  for (auto &material : materials) {
    // < 0 means we need this material
    if (material.first != "ORE" && material.second < 0) {
      // std::cout << "Need " << std::abs(material.second) << " " << material.first << ":" << std::endl;
      const auto reaction = reactions[material.first];
      for (auto &part : reaction.second) {
        // std::cout << "-> Need " << part.first << " " << part.second << ", total ";
        if (materials.find(part.second) == materials.end()) {
          materials[part.second] = 0;
        }
        materials[part.second] = materials[part.second] - part.first;
        // std::cout << materials[part.second] << std::endl;
      }
      material.second += reaction.first;
      // std::cout << "->> Amount " << material.first << " now " << material.second << std::endl;
      return 1;
    }
  }
  return 0;
}

void read_file(const std::string &filename, Reactions &reactions) {
  std::string line;
  std::ifstream file;
  file.open(filename);

  if (!file.is_open())
    return;

  while (std::getline(file, line)) {
    // First split for result
    const auto pos = line.find(" => ");
    std::string result_type;
    int result_count;
    parse_amount(line.substr(pos + 4), result_type, result_count);
    line.erase(pos);

    std::stringstream ss(line);
    std::string field;
    std::vector<Amount> input;
    while (std::getline(ss, field, ',')) {
      std::string input_type;
      int input_count;
      parse_amount(field, input_type, input_count);
      input.push_back(std::make_pair(input_count, input_type));
    }
    reactions[result_type] = std::make_pair(result_count, input);
  }

  file.close();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Need filename" << std::endl;
    return 0;
  }

  Reactions reactions;
  Materials materials;
  read_file(argv[1], reactions);

  materials["FUEL"] = -1;

  while (process_reactions(materials, reactions));

  std::cout << -1 * materials["ORE"] << std::endl;

  return 0;
}
