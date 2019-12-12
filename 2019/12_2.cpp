#include <iostream>
#include <vector>
#include <map>

struct moon {
  moon(int x, int y, int z) : pos{x, y, z}, vel{0, 0, 0} {}
  std::vector<int> pos;
  std::vector<int> vel;
};

int64_t gcd(int64_t a, int64_t b) {
  for (;;) {
    if (a == 0)
      return b;
    b %= a;
    if (b == 0)
      return a;
    a %= b;
  }
}

int64_t lcm(int64_t a, int64_t b) {
  auto temp = gcd(a, b);

  return temp ? (a / temp * b) : 0;
}

int main (int argc, char *argv[]) {
  std::vector<moon> moons;
  std::vector<std::map<std::vector<int>, int>> history;

  history.resize(3);
  moons.push_back(moon(-4, -14, 8));
  moons.push_back(moon(1, -8, 10));
  moons.push_back(moon(-15, 2, 1));
  moons.push_back(moon(-17, -17, 16));

  int t = 0;
  int found[3] = { 0, 0, 0 };

  while (true) {
    // check history
    for (int d = 0; d < 3; d++) {
      if (found[d])
        continue;

      std::vector<int> dim_state;
      dim_state.resize(moons.size() * 2);
      for (int m = 0; m < moons.size(); m++) {
        dim_state[m * 2] = moons[m].pos[d];
        dim_state[m * 2 + 1] = moons[m].vel[d];
      }

      if (history[d].find(dim_state) != history[d].end()) {
        int t_old = history[d][dim_state];
        std::cout << "found cycle for dim " << d << ": " << t - t_old << std::endl;
        found[d] = t - t_old;
      } else {
        history[d][dim_state] = t;
      }
    }

    if (found[0] && found[1] && found[2])
      break;

    // pos
    for (int m1 = 0; m1 < moons.size(); m1++) {
      for (int m2 = m1; m2 < moons.size(); m2++) {
        for (int d = 0; d < 3; d++) {
          if (moons[m1].pos[d] < moons[m2].pos[d]) {
            moons[m1].vel[d]++;
            moons[m2].vel[d]--;
          } else if (moons[m1].pos[d] > moons[m2].pos[d]) {
            moons[m1].vel[d]--;
            moons[m2].vel[d]++;
          }
        }
      }
    }

    // vel
    for (int m = 0; m < moons.size(); m++) {
      for (int d = 0; d < 3; d++) {
        moons[m].pos[d] += moons[m].vel[d];
      }
    }

    // time
    t++;
  }

  std::cout << lcm(lcm(found[0], found[1]), found[2]) << std::endl;

  return 0;
}
