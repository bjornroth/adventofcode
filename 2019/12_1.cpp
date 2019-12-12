#include <iostream>
#include <vector>

struct moon {
  moon(int x, int y, int z) : pos{x, y, z}, vel{0, 0, 0} {}
  std::vector<int> pos;
  std::vector<int> vel;
};

int main (int argc, char *argv[]) {
  std::vector<moon> moons;

  moons.push_back(moon(-4, -14, 8));
  moons.push_back(moon(1, -8, 10));
  moons.push_back(moon(-15, 2, 1));
  moons.push_back(moon(-17, -17, 16));

  for (int t = 0; t < 1000; t++) {
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

    for (int m = 0; m < moons.size(); m++) {
      for (int d = 0; d < 3; d++) {
        moons[m].pos[d] += moons[m].vel[d];
      }
    }

    std::cout << "after " << t << " steps" << std::endl;
    for (int m = 0; m < moons.size(); m++) {
      std::cout << "pos " << moons[m].pos[0] << "," << moons[m].pos[1] << ","
                << moons[m].pos[2] << " vel " << moons[m].vel[0] << ","
                << moons[m].vel[1] << "," << moons[m].vel[2] << std::endl;
    }
  }

  int energy = 0;
  for (int m = 0; m < moons.size(); m++) {
    int pot = 0;
    int kin = 0;
    for (int d = 0; d < 3; d++) {
      pot += abs(moons[m].pos[d]);
      kin += abs(moons[m].vel[d]);
    }
    energy += pot * kin;
  }

  std::cout << energy << std::endl;

  return 0;
}
