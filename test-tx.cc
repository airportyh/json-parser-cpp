#include <iostream>
#include <vector>
#include "json.h"
#include "base-parser.h"
#include "json-parser.h"
#include "json-printer.h"
#include "json-tx.h"
#include "json-tx-printer.h"
#include "json-tx-parser.h"
#include "json-tx-evaluator.h"

using namespace std;

int main() {
    string json = 
    "[ { \"name\": { \"first\": \"Marty\", \"last\": \"Ho\" } }, "
      "{ \"name\": { \"first\": \"Linus\", \"last\": \"Ho\", \"middle\": \"Z\" } }, "
      "{ \"name\": { \"first\": \"Emma\", \"last\": \"Ho\" }, \"oldest\": true } ]";
    std::vector<string> inputs = {
        "name", 
        "[0]", 
        "[name]", 
        "name.first", 
        "[name][first]", 
        "[0].name.first", 
        "0.name.first", 
        "[0][name][first]",
        "[1].name.last",
        "[1].name.first",
        "map(name)",
        "map(name.first)",
        "1 | map(first)",
        "[0].name.first[0]",
        "0.name | map(0)",
        "filter(oldest)",
        "filter(name.middle)",
        "filter(name.middle) | map(name.first)",
        "1",
        "filter(1)",
        "filter(0)",
        "filter(\"abc\")",
        "filter(\"\")",
        "filter(true)",
        "filter(false)",
        "filter(null)",
        "filter(name.first == \"Emma\")"
    };
    JsonParser parser (json);
    JsonValue *jsonValue = parser.parse();
    JsonTxEvaluator evaluater;
    JsonTxPrinter printer;
    JsonPrinter jsonPrinter;
    //JsonTxParser txParser ("name");
    //JsonTxExpression * op = txParser.parse();
    //JsonValue * result = evaluater.evaluate(jsonValue, op);
    //jsonPrinter.print(jsonValue, &cout);
    
    
    for (int i = 0; i < inputs.size(); i++) {
        auto input = inputs[i];
        cout << input << " = ";
        JsonTxParser parser (input);
        try {
            AstNode * op = parser.parse();
            if (op) {
                printer.print(op, &cout);
                JsonValue * result = evaluater.evaluate(jsonValue, op);
                jsonPrinter.print(result, &cout, 1, true);
                cout << endl;
            } else {
                cout << "No parse" << endl;
            }
        } catch (ParseException e) {
            cout << "Parse failed with: " << e.message << endl;
        }
    }
    
} 