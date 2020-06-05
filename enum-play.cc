#include <iostream>
using namespace std;

enum Color { Red = 0, Green, Blue, Yellow };
enum MoreColors { Cyan = 100, Orange, Gold, Magenta };

// enum ExtendedColors { Red, Green, Blue, Yellow };


int main() {
    unsigned int color = Orange;

    cout << color << endl;
}