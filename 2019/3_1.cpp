#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

bool find_pos(const std::pair<int, int> &pos, const std::vector<std::pair<int, int>> &grid) {
  for (int i = 0; i < grid.size(); i++) {
    if (pos == grid[i])
      return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
  std::string line[2];
  std::vector<std::pair<int, int>> grid[2];

  for (int i = 0; i < 2; i++) {
    std::getline(std::cin, line[i]);
    std::stringstream ss(line[i]);
    std::string field;
    int x = 0;
    int y = 0;
    while (std::getline(ss, field, ',')) {
      int dist = std::stoi(field.substr(1));
      for (int d = 0; d < dist; d++) {
        switch (field[0]) {
        case 'R':
          x++;
          break;
        case 'L':
          x--;
          break;
        case 'U':
          y++;
          break;
        case 'D':
          y--;
          break;
        }
        grid[i].push_back(std::make_pair(x,y));
      }
    }
  }

  int min_dist = INT_MAX;
  for (int i = 0; i < grid[0].size(); i++) {
    if (find_pos(grid[0][i], grid[1])) {
      int dist = std::abs(grid[0][i].first) + std::abs(grid[0][i].second);
      std::cout << "cross at " << grid[0][i].first << "," << grid[0][i].second << " dist " << dist << std::endl;
      if (dist < min_dist)
        min_dist = dist;
    }
  }

  std::cout << "min dist " << min_dist << std::endl;

  return 0;
}

