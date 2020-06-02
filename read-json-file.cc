#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include "json.h"
#include "json-parser.h"
#include "json-printer.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "Please provide a file name." << endl;
        return 0;
    }
    
    char *filename = argv[1];
    
    std::ifstream t(filename);
    std::string input((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    JsonParser parser(input);
    JsonValue *result = parser.parse();
    JsonPrinter printer;
    printer.print(result, &cout);
}