#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    int code = 966;
    int part1 = 0b11000000 + ((0b11111000000 & code) >> 6);
    int part2 = 0b10000000 +  (0b00000111111 & code);
    cout << to_string(part1) << ": " << to_string(part2) << endl;
}