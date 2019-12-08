#include <iostream>
#include <fstream>
#include <vector>

std::string read_file(const std::string &filename) {
  std::string input;
  std::ifstream file;

  file.open(filename);
  if (!file.is_open())
    return "";

  std::getline(file, input);
  file.close();
  return input;
}

bool read_layer(const std::string &input, int width, int height, std::vector<int> &layer) {
  int size = width * height;
  if (input.size() < size)
    return false;

  for (int i = 0; i < size; i++) {
    layer.push_back(std::stoi(std::string(1, input[i])));
  }

  return true;
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 0;

  auto input = read_file(argv[1]);
  std::vector<std::vector<int>> layers;

  int num_layers = 0;
  int min_zeroes = INT_MAX;
  int min_layer_idx = -1;
  std::vector<int> layer;
  while (read_layer(input, 25, 6, layer)) {
    input = input.substr(layer.size());
    layers.push_back(layer);
    int c = std::count(layer.begin(), layer.end(), 0);
    if (c < min_zeroes) {
      min_zeroes = c;
      min_layer_idx = num_layers;
    }
    layer.clear();
    num_layers++;
  }

  if (min_layer_idx < 0)
    return 0;

  std::cout << "num layers " << layers.size() << " min layer " << min_layer_idx << std::endl;

  const auto &min_layer = layers[min_layer_idx];
  int ones = std::count(min_layer.begin(), min_layer.end(), 1);
  int twos = std::count(min_layer.begin(), min_layer.end(), 2);

  std::cout << ones * twos << std::endl;

  return 0;
}
