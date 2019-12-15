#include <iostream>
#include <map>
#include <stack>
#include <stdlib.h>
#include "intcode.h"

#define NORTH 1
#define SOUTH 2
#define WEST 3
#define EAST 4

#define WALL 0
#define MOVED 1
#define VISITED 1
#define MOVED_FOUND 2
#define VISITED_FOUND 2

typedef std::pair<int, int> Coord;
typedef std::map<Coord, int> Map;
typedef std::map<Coord, std::vector<int>> Moves;
typedef std::stack<int> Path;
typedef std::pair<Coord, int> Node;

Coord update_position(const Coord& pos, int dir) {
  Coord updated;
  switch (dir) {
  case NORTH:
    updated = std::make_pair(pos.first, pos.second - 1);
    break;
  case SOUTH:
    updated = std::make_pair(pos.first, pos.second + 1);
    break;
  case WEST:
    updated = std::make_pair(pos.first - 1, pos.second);
    break;
  case EAST:
    updated = std::make_pair(pos.first + 1, pos.second);
    break;
  }
  return updated;
}

int direction_from_delta(int delta_x, int delta_y) {
  if (delta_x > 0)
    return EAST;
  if (delta_x < 0)
    return WEST;
  if (delta_y > 0)
    return SOUTH;
  if (delta_y < 0)
    return NORTH;
  return 0;
}

int reverse_direction(int dir) {
  switch (dir) {
  case NORTH:
    return SOUTH;
  case SOUTH:
    return NORTH;
  case WEST:
    return EAST;
  case EAST:
    return WEST;
  }
  return 0;
}

void probe_map(Intcode &robot, Map &map, Moves &moves, const Coord &pos) {
  for (int dir = NORTH; dir <= EAST; dir++) {
    Coord test_pos = update_position(pos, dir);
    if (map.find(test_pos) == map.end()) {
      IO in, out;
      in.push_back(dir);
      robot.run(in, out);
      map[test_pos] = out[0];
      if (out[0] != WALL) {
        out.pop_front();
        moves[pos].push_back(dir);
        in.push_back(reverse_direction(dir));
        robot.run(in, out);
      }
    }
  }
}

void move_robot(Intcode &robot, Map &map, Coord &robot_pos, int dir) {
  IO in, out;
  in.push_back(dir);
  robot.run(in, out);
  robot_pos = update_position(robot_pos, dir);
}

void draw_map(Map &map, Coord &robot_pos) {
  int min_x = 0;
  int max_x = 0;
  int min_y = 0;
  int max_y = 0;
  for (const auto &pos : map) {
    min_x = std::min(pos.first.first, min_x);
    max_x = std::max(pos.first.first, max_x);
    min_y = std::min(pos.first.second, min_y);
    max_y = std::max(pos.first.second, max_y);
  }

  system("clear");
  for (int y = min_y - 1; y <= max_y + 1; y++) {
    for (int x = min_x - 1; x <= max_x + 1; x++) {
      if (x == robot_pos.first && y == robot_pos.second) {
        std::cout << "D";
      } else if (map.find(std::make_pair(x, y)) != map.end()) {
        switch (map[std::make_pair(x, y)]) {
        case WALL:
          std::cout << "#";
          break;
        case VISITED:
          std::cout << ".";
          break;
        case VISITED_FOUND:
          std::cout << "!";
          break;
        }
      } else {
        std::cout << "?";
      }
    }
    std::cout << std::endl;
  }
}

int find_shortest(Coord &pos, Map &map) {
  static int rowNum[] = {-1, 0, 0, 1};
  static int colNum[] = {0, -1, 1, 0};
  std::queue<Node> bfs_queue;
  std::map<Coord, bool> visited;
  bfs_queue.push(std::make_pair(pos, 0));
  while (!bfs_queue.empty()) {
    auto curr = bfs_queue.front();
    bfs_queue.pop();

    auto pt = curr.first;
    if (pt.first == 0 && pt.second == 0)
      return curr.second;

    for (int i = 0; i < 4; i++) {
      Coord test_pos;
      test_pos.first = pt.first + colNum[i];
      test_pos.second = pt.second + rowNum[i];
      if (map.find(test_pos) != map.end() && map[test_pos] != WALL &&
          visited.find(test_pos) == visited.end()) {
        visited[test_pos] = true;
        auto adj_cell = std::make_pair(test_pos, curr.second + 1);
        bfs_queue.push(adj_cell);
      }
    }
  }
  return -1;
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 0;

  Memory memory;
  IO in, out;
  Intcode::read_memory(argv[1], memory);
  Intcode robot(memory);
  Map robot_map;
  Path path;
  Moves moves;

  int robot_direction = NORTH;
  Coord robot_pos = std::make_pair(0, 0);
  bool found = false;
  robot_map[robot_pos] = VISITED;

  while (!found) {
    probe_map(robot, robot_map, moves, robot_pos);
    // draw_map(robot_map, robot_pos);
    Coord new_pos;
    if (moves[robot_pos].empty()) {
      // backtrack
      int dir = path.top();
      path.pop();
      move_robot(robot, robot_map, robot_pos, reverse_direction(dir));
    } else {
      // DFS, or whatever
      int dir = moves[robot_pos][0];
      moves[robot_pos].erase(moves[robot_pos].begin());
      move_robot(robot, robot_map, robot_pos, dir);
      path.push(dir);
    }
    found = (robot_map[robot_pos] == VISITED_FOUND);
  }

  std::cout << find_shortest(robot_pos, robot_map) << std::endl;

  return 0;
}
