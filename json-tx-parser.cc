#include <iostream>
#include <vector>
#include "json.h"
#include "base-parser.h"
#include "json-parser.h"
#include "json-printer.h"

using namespace std;

enum JsonTxType {
    JsonTxPathExpression = 100,       // [0].partner[name].first
    JsonTxCallExpression,             // map(name)
    JsonTxPipeExpression,             // team.people | filter(isCool) | map(name)
    JsonTxBinaryOpExpression,         // filter(age > 3) | map(name)
    JsonTxConditionExpression,        // map(age > 3 ? name + "3+" : name + "toddler")
};

enum JsonTxBinaryOperator {
    JsonTxPlus,
    JsonTxMinus,
    JsonTxTimes,
    JsonTxDivide,
    JsonTxGreaterThan,
    JsonTxLessThan,
    JsonTxGreaterThanOrEqual,
    JsonTxLessThanOrEqual,
    JsonTxEqual
};

class JsonTxPathExpressionNode: public AstNode {
    public:
    std::vector<string> *path;

    JsonTxPathExpressionNode(std::vector<string> * p): 
        path(p), 
        AstNode(JsonTxPathExpression) {};
};

class JsonTxCallExpressionNode: public AstNode {
    public:
    string funName;
    class AstNode *fun;

    JsonTxCallExpressionNode(string fn, AstNode *f): 
        funName(fn), 
        fun(f), 
        AstNode(JsonTxCallExpression) {};
};

class JsonTxPipeExpressionNode: public AstNode {
    public:
    std::vector<AstNode *> *operations;

    JsonTxPipeExpressionNode(std::vector<AstNode *> *ops): 
        operations(ops), 
        AstNode(JsonTxPipeExpression) {};
};

class JsonTxBinaryOpExpressionNode: public AstNode {
    public:
    AstNode *left;
    JsonTxBinaryOperator op;
    AstNode *right;

    JsonTxBinaryOpExpressionNode(
        AstNode *l,
        JsonTxBinaryOperator o,
        AstNode *r
    ): 
        left(l),
        op(o),
        right(r),
        AstNode(JsonTxBinaryOpExpression) {};
};

class JsonTxConditionExpressionNode: public AstNode {
    public:
    AstNode *condition;
    AstNode *consequent;
    AstNode *alternate;

    JsonTxConditionExpressionNode(
        AstNode *cn,
        AstNode *cs,
        AstNode *a
    ): 
        condition(cn),
        consequent(cs),
        alternate(a),
        AstNode(JsonTxConditionExpression) {};
};

class JsonTxPrinter {
    public:
    void print(AstNode *op, ostream *out, int level = 0) {
        if (op->type == JsonTxPathExpression) {
            JsonTxPathExpressionNode *pathExpr = static_cast<JsonTxPathExpressionNode *>(op);
            std::vector<string> *pathSegments = pathExpr->path;
            for (int i = 0; i < pathSegments->size(); i++) {
                string prop = (*pathSegments)[i];
                if (i > 0) {
                    *out << ".";
                }
                *out << prop;
            }
        } else if (op->type == JsonTxPipeExpression) {
            JsonTxPipeExpressionNode *pipeExpr = static_cast<JsonTxPipeExpressionNode *>(op);
            std::vector<AstNode *> *ops = pipeExpr->operations;
            for (int i = 0; i < ops->size(); i++) {
                if (i > 0) {
                    *out << " | ";
                }
                auto childOp = (*ops)[i];
                print(childOp, out, level + 1);
            }
        } else if (op->type == JsonTxCallExpression) {
            JsonTxCallExpressionNode *callExpr = static_cast<JsonTxCallExpressionNode *>(op);
            *out << callExpr->funName << "(";
            print(callExpr->fun, out, level + 1);
            *out << ")";
        } else {
            throw "blah";
        }
        if (level == 0) {
            *out << endl;
        }
    }
};

class JsonTxParser: public BaseParser {
    public:
    JsonTxParser(string input): BaseParser(input) {}

    /*
    op -> pipe_op
    */
    AstNode * parse() {
        return parsePipe();
    }

    /*
    pipe_op -> non_pipe_op
             | non_pipe_op _ "|" _ pipe_op
    */
    AstNode * parsePipe() {
        std::vector<AstNode *> *ops = new std::vector<AstNode *>();
        while (true) {
            AstNode *op = parseNonPipeOperation();
            if (op) {
                ops->push_back(op);
            } else {
                throw ParseException("Expected an operation expression but unable to find one.");
            }
            parseWs();
            if (input[cursor] == '|') {
                cursor++;
                parseWs();
                // loop and parse another
            } else {
                break;
            }
        }
        return new JsonTxPipeExpressionNode(ops);
    }

    /*
    non_pipe_op -> map_op
                 | filter_op
                 | path_op
                 | binary_op
                 | condition_op
    */
    AstNode * parseNonPipeOperation() {
        int cursorStart = cursor;
        AstNode *result;
        result = parseMapOperation();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseFilterOperation();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parsePathOperation();
        if (result) {
            return result;
        }
        /*
        cursor = cursorStart;
        result = parseBinaryOperation();
        if (result) {
            return result;
        }*/
        return NULL;
        /*
        cursor = cursorStart;
        return parseConditionOperation();*/
    }

    /*
        map_op -> "map(" _ op _ ")"
    */
    AstNode *parseMapOperation() {
        if (input.substr(cursor, 4) == "map(") {
            cursor += 4;
            parseWs();
            AstNode *pipe = parsePipe();
            if (pipe) {
                parseWs();
                if (input[cursor] == ')') {
                    cursor++;
                    return new JsonTxCallExpressionNode("map", pipe);
                }
            }
        }
        return NULL;
    }

    /*
        filter_op -> "filter(" _ op _ ")"
    */
    AstNode *parseFilterOperation() {
        if (input.substr(cursor, 7) == "filter(") {
            cursor += 7;
            parseWs();
            AstNode *pipe = parsePipe();
            if (pipe) {
                parseWs();
                if (input[cursor] == ')') {
                    cursor++;
                    return new JsonTxCallExpressionNode("filter", pipe);
                }
            }
        }
        return NULL;
    }

    /*
    path_op -> path_op_first
             | path_op_first path_op_rest
    
    path_op_first -> identifier
                   | "[" identifier "]"

    path_op_rest -> path_op_non_first
                  | path_op_non_first path_op_rest

    path_op_non_first -> "[" identifier "]"
                       | "." identifier
    */
    AstNode * parsePathOperation() {
        std::vector<string> *props = new std::vector<string>();
        if (parsePathOperationFirst(props)) {
            while (true) {
                if (!parsePathOperationNonFirst(props)) {
                    break;
                }
            }
            return new JsonTxPathExpressionNode(props);
        } else {
            return NULL;
        }
    }

    bool parsePathOperationFirst(std::vector<string> *props) {
        int cursorStart = cursor;
        string *prop = parseIdentifier();
        if (prop) {
            props->push_back(*prop);
            return true;
        }
        cursor = cursorStart;
        prop = parseIndexNotation();
        if (prop) {
            props->push_back(*prop);
            return true;
        }
        return false;
    }

    bool parsePathOperationNonFirst(std::vector<string> *props) {
        int cursorStart = cursor;
        if (input[cursor] == '.') {
            cursor++;
            string *prop = parseIdentifier();
            if (prop) {
                props->push_back(*prop);
                return true;
            }
        }
        cursor = cursorStart;
        string *prop = parseIndexNotation();
        if (prop) {
            props->push_back(*prop);
            return true;
        }
        return false;
    }

    /*
        identifier -> alphanum
                    | alphanum identifier
    */
    string * parseIdentifier() {
        string identifier = "";
        // alpha-numeric
        while (true) {
            char chr = input[cursor];
            if ((chr >= 48 && chr <= 57) || (chr >= 65 && chr <= 90) || (chr >= 97 && chr <= 122)) {
                identifier.push_back(chr);
                cursor++;
            } else {
                break;
            }
        }
        if (identifier.size() > 0) {
            return new string(identifier);
        }
        return NULL;
    }

    /*
        index_notation -> "[" identifier "]"
    */
    string * parseIndexNotation() {
        if (input[cursor] == '[') {
            cursor++;
            string * prop = parseIdentifier();
            if (prop) {
                if (input[cursor] == ']') {
                    cursor++;
                    return prop;
                }
            } else {
                throw ParseException("Expected an identifier for index notation.");
            }
        }
        return NULL;
    }

};

class JsonTxEvaluator {
    public:
    JsonValue * evaluate(JsonValue *value, AstNode *op) {
        if (op->type == JsonTxPipeExpression) {
            JsonTxPipeExpressionNode *pipeExpr = static_cast<JsonTxPipeExpressionNode *>(op);
            auto ops = pipeExpr->operations;
            auto currentValue = value;
            for (int i = 0; i < ops->size(); i++) {
                auto childOp = (*ops)[i];
                currentValue = evaluate(currentValue, childOp);
            }
            return currentValue;
        } else if (op->type == JsonTxPathExpression) {
            JsonTxPathExpressionNode *pathExpr = static_cast<JsonTxPathExpressionNode *>(op);
            auto path = pathExpr->path;
            auto currentValue = value;
            for (int i = 0; i < path->size(); i++) {
                string key = (*path)[i];
                currentValue = indexedLookup(currentValue, &key);
            }
            return currentValue;
        } else if (op->type == JsonTxCallExpression) {
            JsonTxCallExpressionNode *callExpr = static_cast<JsonTxCallExpressionNode *>(op);
            if (callExpr->funName == "map") {
                return performMap(value, callExpr->fun);
            } else if (callExpr->funName == "filter") {
                return performFilter(value, callExpr->fun);
            }
        }
        return JsonValue::null();
    }

    JsonValue *performMap(JsonValue *value, AstNode *op) {
        if (value->type == JsonArray) {
            std::vector<JsonValue *> *array = value->array();
            std::vector<JsonValue *> *resultArray = new std::vector<JsonValue *>();
            for (int i = 0; i < array->size(); i++) {
                auto item = (*array)[i];
                resultArray->push_back(evaluate(item, op));
            }
            return JsonValue::array(resultArray);
        } else if (value->type == JsonObject) {
            std::map<string, JsonValue *> *map = value->object();
            std::map<string, JsonValue *> *resultMap = new std::map<string, JsonValue *>();
            for (auto it = map->begin(); it != map->end(); it++) {
                (*resultMap)[it->first] = evaluate(it->second, op);
            }
            return JsonValue::object(resultMap);
        }  else {
            return JsonValue::array(new std::vector<JsonValue *>());
        }
    }

    JsonValue *performFilter(JsonValue *value, AstNode *op) {
        if (value->type == JsonArray) {
            std::vector<JsonValue *> *array = value->array();
            std::vector<JsonValue *> *resultArray = new std::vector<JsonValue *>();
            for (int i = 0; i < array->size(); i++) {
                auto item = (*array)[i];
                if (evaluate(item, op)->isTruthy()) {
                    resultArray->push_back(item);
                }
            }
            return JsonValue::array(resultArray);
        } else if (value->type == JsonObject) {
            std::map<string, JsonValue *> *map = value->object();
            std::map<string, JsonValue *> *resultMap = new std::map<string, JsonValue *>();
            for (auto it = map->begin(); it != map->end(); it++) {
                auto result = evaluate(it->second, op);
                if (result->isTruthy()) {
                    (*resultMap)[it->first] = it->second;
                }
            }
            return JsonValue::object(resultMap);
        }  else {
            return JsonValue::array(new std::vector<JsonValue *>());
        }
    }

    JsonValue * indexedLookup(JsonValue *value, string *key) {
        if (value->type == JsonObject) {
            std::map<string, JsonValue *> *map = value->object();
            if (map->find(*key) != map->end()) {
                return (*map)[*key];
            } else {
                return JsonValue::null();
            }
        } else if (value->type == JsonArray) {
            int index;
            try {
                index = stoi(*key);
            } catch (std::invalid_argument e) {
                return JsonValue::null();
            }
            auto vector = value->array();
            auto value = (*vector)[index];
            if (value) {
                return value;
            } else {
                return JsonValue::null();
            }
        } else if (value->type == JsonString) {
            int index;
            try {
                index = stoi(*key);
            } catch (std::invalid_argument e) {
                return JsonValue::null();
            }
            auto string = value->string();
            return JsonValue::string(string.substr(index, 1));
        } else {
            return JsonValue::null();
        }
    }
};

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
        "filter(name.middle) | map(name.first)"
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