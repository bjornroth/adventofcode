#include <iostream>

int main(int argc, char *argv[]) {
  int count = 0;

  for (int code = 171309; code <= 643603; code++) {
    int digits[6];
    int digit = 0;
    int cc = code;

    while (cc != 0) {
      digits[digit++] = cc % 10;
      cc = cc / 10;
    }

    bool dd = false;
    bool mono = true;

    for (int d = 0; d < 5; d++) {
      dd |= (digits[d] == digits[d+1]);
      mono &= (digits[d] >= digits[d+1]);
    }

    if (dd && mono) {
      count++;
    }
  }

  std::cout << count << std::endl;
}
