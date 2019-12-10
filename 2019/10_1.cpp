#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>

struct Point {
  int x;
  int y;
};

struct Vector {
  Point pos;
  double s;
  double v;
};

typedef std::vector<Point> Map;
typedef std::vector<Vector> Vectors;

void read_file(const std::string &filename, Map &map) {
  std::string line;
  std::ifstream file;
  file.open(filename);

  if (!file.is_open())
    return;

  int y = 0;
  while (std::getline(file, line)) {
    for (int x = 0; x < line.size(); x++) {
      if (line[x] == '#') {
        Point p;
        p.x = x;
        p.y = y;
        map.push_back(p);
      }
    }
    y++;
  }

  file.close();
}

Vectors calc_all_vectors(const Point &point, const Map &map) {
  Vectors vectors;
  for (const auto &pos : map) {
    if (pos.x == point.x && pos.y == point.y)
      continue;
    int dx = point.x - pos.x;
    int dy = point.y - pos.y;
    Vector v;
    v.pos = pos;
    v.s = std::sqrt(dx*dx + dy*dy);
    v.v = std::atan2(dx, dy);
    vectors.push_back(v);
  }
  return vectors;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "file name missing" << std::endl;
  }

  Map map;
  read_file(argv[1], map);
  int max_count = 0;
  Point max_pos;

  for (const auto &pos : map) {
    Vectors vectors = calc_all_vectors(pos, map);
    std::sort(vectors.begin(), vectors.end(), [](Vector a, Vector b) {
      if (a.v == b.v) // fishy
        return a.s < b.s;
      else
        return a.v < b.v;
    });

    // std::cout << "pos " << pos.x << "," << pos.y << ":" << std::endl;
    // for (const auto &v : vectors) {
    //   std::cout << "  " << v.pos.x << "," << v.pos.y << ": " << v.v << " " << v.s << std::endl;
    // }

    auto last = std::unique(vectors.begin(), vectors.end(),
                            [](Vector a, Vector b) { return a.v == b.v; });
    vectors.erase(last, vectors.end());
    // std::cout << vectors.size() << std::endl;
    int count = std::max(max_count, (int) vectors.size());
    if (count > max_count) {
      max_count = count;
      max_pos = pos;
    }
  }

  std::cout << max_pos.x << "," << max_pos.y << ": " << max_count << std::endl;

  return 0;
}
