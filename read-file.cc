#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    string line;
    ifstream myfile ("unicode.txt");
    getline (myfile,line);
    cout << "number of characters: " << line.size() << endl;
    for (int i = 0; i < line.size(); i++) {
        char code = line[i];
        unsigned char ucode = line[i];
        cout << line[i] << "\t" << to_string(ucode) << "\t" << std::bitset<8>(ucode) << endl;
    }
    cout << line << '\n';
    myfile.close();
}