#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    int code = 916;
    int part1 = 0b11000000 + ((0b11111000000 & code) >> 6);
    int part2 = 0b10000000 +  (0b00000111111 & code);
    cout << to_string(part1) << ": " << to_string(part2) << endl;
    
    string line = "Δ";
    for (int i = 0; i < line.size(); i++) {
        char code = line[i];
        unsigned char ucode = line[i];
        cout << i << ": " << to_string(code) << ": " << to_string(ucode) << ": " << line[i] << endl;
    }
    cout << line << '\n';
}