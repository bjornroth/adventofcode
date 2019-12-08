#include <iostream>
#include <fstream>
#include <vector>

#define WIDTH  25
#define HEIGHT 6

#define BLACK  0
#define WHITE  1
#define TRANSP 2

typedef std::vector<int> Layer;
typedef std::vector<Layer> Image;

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

bool read_layer(const std::string &input, int width, int height, Layer &layer) {
  int size = width * height;
  if (input.size() < size)
    return false;

  for (int i = 0; i < size; i++) {
    layer.push_back(std::stoi(std::string(1, input[i])));
  }

  return true;
}

Image read_image(const std::string &filename) {
  auto input = read_file(filename);
  Image layers;
  Layer layer;
  while (read_layer(input, WIDTH, HEIGHT, layer)) {
    input = input.substr(layer.size());
    layers.push_back(layer);
    layer.clear();
  }
  return layers;
}

void print_image(const Image &image, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      for (int l = 0; l < image.size(); l++) {
        int pixval = image[l][y * width + x];
        if (pixval == BLACK) {
          std::cout << " ";
          break;
        } else if (pixval == WHITE) {
          std::cout << "X";
          break;
        }
      }
    }
    std::cout << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 0;

  const auto image = read_image(argv[1]);
  print_image(image, WIDTH, HEIGHT);

  return 0;
}
