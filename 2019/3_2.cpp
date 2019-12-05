#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

int find_pos(const std::pair<int, int> &pos, const std::vector<std::pair<int, int>> &grid) {
  for (int i = 0; i < grid.size(); i++) {
    if (pos == grid[i])
      return i;
  }
  return -1;
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

  int min_step_sum = INT_MAX;
  for (int step = 0; step < grid[0].size(); step++) {
    int other_step = find_pos(grid[0][step], grid[1]);
    if (other_step >= 0) {
      int step_sum = (step + 1) + (other_step + 1);
      std::cout << "cross at " << grid[0][step].first << "," << grid[0][step].second << " step sum " << step_sum << std::endl;
      if (step_sum < min_step_sum)
        min_step_sum = step_sum;
    }
  }

  std::cout << "min step sum " << min_step_sum << std::endl;

  return 0;
}

