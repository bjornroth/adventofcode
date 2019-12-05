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
      if ((d == 0 && digits[0] == digits[1] && digits[1] != digits[2]) ||
          (d == 4 && digits[4] == digits[5] && digits[4] != digits[3]) ||
          (digits[d] == digits[d+1] && digits[d-1] != digits[d] && digits[d+2] != digits[d])) {
        dd = true;
      }
      mono &= (digits[d] >= digits[d+1]);
    }

    if (dd && mono) {
      count++;
    }
  }

  std::cout << count << std::endl;
}
