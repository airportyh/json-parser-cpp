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
        cout << i << ": " << to_string(code) << ": " << to_string(ucode) << ": " << line[i] << endl;
    }
    cout << line << '\n';
    myfile.close();

    string my_string;
    my_string.push_back(-28);
    my_string.push_back(-67);
    my_string.push_back(-96);
    cout << my_string << endl;
    string me = "我";
    cout << me << endl;
}