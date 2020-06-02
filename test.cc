#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <exception>
#include "json.h"
#include "json-parser.h"
#include "json-printer.h"

using namespace std;

int main() {
    vector<string> testCases {
        /*"12.56", 
        "-67", 
        "12.45e2", 
        "12.45e-2", 
        "12.45e+2", 
        "-1", 
        "0", 
        "-0", 
        "00",
        "0.",
        "\"abc\"",
        "\"I really \\\"like\\\" her\"",
        "\"abc\tdef\"",
        "[1, 2, 3]",
        "[1, 2]",
        "[1]",
        "[\"abc\", \"def\"]",
        "[]",*/
        "[[[[5]]], []]",
        /*"true",
        "[true, false]",
        "{}",
        "{ \"name\": \"Bob\" }",
        "{ \"name\": \"Bob\", \"age\": 17, \"friends\": [] }",
        "{}",
        "{ \"numbers\": [1, 2, 3, 4] }",
        "{ \"numb\ters\": [1, 2, 3, 4] }",
        "\"\\u1234\"",
        "\\u1234",
        "I really like her",
        "\"3.66\\u03c0\"",
        "\"我\""*/
    };
    JsonPrinter printer;

    for (int i = 0; i < testCases.size(); i++) {
        string input = testCases[i];
        JsonParser parser(input);
        cout << input << " = ";
        try {
            auto result = parser.parse();
            if (result) {
                printer.print(result, &cout);
                cout << ", " << parser.cursor << endl;
            } else {
                cout << "no parse found" << endl;
            }
        } catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    
}