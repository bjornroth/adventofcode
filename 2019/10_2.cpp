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
    // convert angle to fit laser
    if (v.v != 0.0)
      v.v *= -1.0;
    if (v.v < 0.0)
      v.v += 2*M_PI;
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
  Vectors max_vectors;

  for (const auto &pos : map) {
    auto vectors = calc_all_vectors(pos, map);
    std::sort(vectors.begin(), vectors.end(), [](Vector a, Vector b) {
      if (a.v == b.v) // fishy
        return a.s < b.s;
      else
        return a.v < b.v;
    });

    const auto orig_vectors = vectors;
    auto last = std::unique(vectors.begin(), vectors.end(),
                            [](Vector a, Vector b) { return a.v == b.v; });
    vectors.erase(last, vectors.end());

    int count = std::max(max_count, (int) vectors.size());
    if (count > max_count) {
      max_count = count;
      max_pos = pos;
      max_vectors = orig_vectors;
    }
  }

  std::cout << "max: " << max_pos.x << "," << max_pos.y << ": " << max_count << std::endl;

  int vapor_count = 0;
  while (!max_vectors.empty()) {
    double last_angle = 1000; // ?
    auto it = max_vectors.begin();
    while (it != max_vectors.end()) {
      if (it->v != last_angle) {
        vapor_count++;
        if (vapor_count == 200)
          std::cout << vapor_count << " " << it->pos.x << "," << it->pos.y << ": " << it->v << std::endl;
        last_angle = it->v;
        it = max_vectors.erase(it);
      } else {
        ++it;
      }
    }
  }

  return 0;
}
