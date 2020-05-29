#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <exception>

using namespace std;

class JsonValue {
    public:
    virtual bool isObject() = 0;
    virtual bool isString() = 0;
    virtual bool isArray() = 0;
    virtual bool isNumber() = 0;
    virtual bool isBoolean() = 0;
    virtual bool isNull() = 0;
    virtual string repr() = 0;
};

class JsonObject : public JsonValue {
    public:
    std::map<string, JsonValue *> *object;

    JsonObject(std::map<string, JsonValue *> *obj): object(obj) {}

    bool isObject() { return true; }
    bool isString() { return false; }
    bool isArray() { return false; }
    bool isNumber() { return false; }
    bool isBoolean() { return false; }
    bool isNull() { return false; }
    string repr() { return "<object>"; }
};

class JsonString : public JsonValue {
    public:
    string _string;

    JsonString(string str): _string(str) {}

    bool isObject() { return false; }
    bool isString() { return true; }
    bool isArray() { return false; }
    bool isNumber() { return false; }
    bool isBoolean() { return false; }
    bool isNull() { return false; }
    string repr() { return "\"" + _string + "\""; }
};

class JsonArray : public JsonValue {
    public:
    vector<JsonValue *> *array;

    JsonArray(vector<JsonValue *> *arr): array(arr) {}

    bool isObject() { return false; }
    bool isString() { return false; }
    bool isArray() { return true; }
    bool isNumber() { return false; }
    bool isBoolean() { return false; }
    bool isNull() { return false; }
    string repr() { return "<array(" + to_string(array->size()) + ")>"; }
};

class JsonNumber : public JsonValue {
    public:
    double number;

    JsonNumber(double _number): number(_number) {}

    bool isObject() { return false; }
    bool isString() { return false; }
    bool isArray() { return false; }
    bool isNumber() { return true; }
    bool isBoolean() { return false; }
    bool isNull() { return false; }
    string repr() { return to_string(number); }
};

class JsonBoolean : public JsonValue {
    public:
    double boolean;

    JsonBoolean(bool _bool): boolean(_bool) {}

    bool isObject() { return false; }
    bool isString() { return false; }
    bool isArray() { return false; }
    bool isNumber() { return false; }
    bool isBoolean() { return true; }
    bool isNull() { return false; }
    string repr() {
        if (boolean) {
            return "true";
        } else {
            return "false";
        }
    }
};

class JsonNull : public JsonValue {
    public:
    JsonNull() {}

    bool isObject() { return false; }
    bool isString() { return false; }
    bool isArray() { return false; }
    bool isNumber() { return false; }
    bool isBoolean() { return false; }
    bool isNull() { return true; }
    string repr() { return "null"; }
};

class ParseException : public exception {
    public:
    string message;

    ParseException(string msg): message(msg) {}

    const char* what() const throw()
    {
        return message.c_str();
    }
};

class JsonPrinter {
    public:
    void print(JsonValue *value, ostream *out) {
        if (value->isNumber()) {
            *out << to_string(((JsonNumber *)value)->number);
        } else if (value->isString()) {
            string str = ((JsonString *)value)->_string;
            quote(str);
            *out << str;
        } else if (value->isNull()) {
            *out << "null";
        } else if (value->isBoolean()) {
            if (((JsonBoolean *)value)->boolean) {
                *out << "true";
            } else {
                *out << "false";
            }
        } else if (value->isArray()) {
            *out << "[ ";
            vector<JsonValue *> *array = ((JsonArray *)value)->array;
            for (int i = 0; i < array->size(); i++) {
                JsonValue *value = (*array)[i];
                if (i > 0) {
                    *out << ", ";
                }
                print(value, out);
            }
            *out << " ]";
        } else if (value->isObject()) {
            std::map<string, JsonValue *> *map = ((JsonObject *)value)->object;
            auto it = map->begin();
            *out << "{ ";
            while (it != map->end()) {
                string str = it->first;
                quote(str);
                *out << str << ": ";
                print(it->second, out);
                it++;
            }
            *out << " }";
        }
    }

    // https://stackoverflow.com/questions/1494399/how-do-i-search-find-and-replace-in-a-standard-string
    void replaceAll(std::string& str,
                const std::string& oldStr,
                const std::string& newStr)
    {
        std::string::size_type pos = 0u;
        while((pos = str.find(oldStr, pos)) != std::string::npos){
            str.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
    }

    void quote(string& str) {
        replaceAll(str, "\"", "\\\"");
        replaceAll(str, "\t", "\\t");
        replaceAll(str, "\n", "\\n");
        replaceAll(str, "\f", "\\f");
        replaceAll(str, "\b", "\\b");
        replaceAll(str, "\r", "\\r");
        str.insert(0, "\"");
        str.push_back('"');
    }
};

class JsonParser {
    public:
    int cursor;
    string input;

    JsonParser(string _input): input(_input), cursor(0) {}

    JsonValue *parse() {
        return parseElement();
    }

    JsonValue *parseValue() {
        int cursorStart = cursor;
        JsonValue *result;
        result = parseNull();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseBoolean();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseArray();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseObject();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseString();
        if (result != NULL) {
            return result;
        }
        cursor = cursorStart;
        return parseNumber();
    }

    JsonValue *parseNull() {
        if (input.substr(cursor, 4) == "null") {
            cursor += 4;
            return new JsonNull();
        } else {
            return NULL;
        }
    }

    JsonValue *parseBoolean() {
        if (input.substr(cursor, 4) == "true") {
            cursor += 4;
            return new JsonBoolean(true);
        } else if (input.substr(cursor, 5) == "false") {
            cursor += 5;
            return new JsonBoolean(false);
        } else {
            return NULL;
        }
    }

    JsonValue *parseObject() {
        if (input[cursor] == '{') {
            cursor++;
            int cursorStart = cursor;
            std::map<string, JsonValue *> *map = parseMembers();
            if (map) {
                if (input[cursor] == '}') {
                    cursor++;
                    return new JsonObject(map);
                } else {
                    throw ParseException("Expected closing bracket for an object.");
                }
            } else {
                cursor = cursorStart;
                parseWs();
                if (input[cursor] == '}') {
                    cursor++;
                    std::map<string, JsonValue *> *map = new std::map<string, JsonValue *>();
                    return new JsonObject(map);
                } else {
                    throw ParseException("Expected closing bracket for an object.");
                }
            }
        } else {
            return NULL;
        }
    }

    std::map<string, JsonValue *> *parseMembers() {
        int cursorStart = cursor;
        std::map<string, JsonValue *> *map = new std::map<string, JsonValue *>();
        if (parseMember(map)) {
            while (true) {
                if (input[cursor] == ',') {
                    cursor++;
                    if (!parseMember(map)) {
                        throw ParseException("Expected an object entry following a comma.");
                    }
                } else {
                    break;
                }
            }
        } else {
            cursor = cursorStart;
            parseWs();
        }
        return map;
    }

    bool parseMember(map<string, JsonValue *> *map) {
        parseWs();
        JsonString *key = parseString();
        if (key) {
            parseWs();
            if (input[cursor] == ':') {
                cursor++;
                JsonValue *element = parseElement();
                if (element) {
                    (*map)[key->_string] = element;
                    delete key;
                    return true;
                }
            }
        }
        return false;
    }

    JsonValue *parseArray() {
        int cursorStart = cursor;
        if (input[cursor] == '[') {
            cursor++;
            vector<JsonValue *> *elements = parseElements();
            if (elements) {
                if (input[cursor] == ']') {
                    cursor++;
                    return new JsonArray(elements);
                } else {
                    throw ParseException("Expected closing bracket for an array.");
                }
            }
        }
        return NULL;
    }

    vector<JsonValue *> *parseElements() {
        int cursorStart = cursor;
        vector<JsonValue *> *elements = new vector<JsonValue *>();
        JsonValue *firstElement = parseElement();
        if (firstElement) {
            elements->push_back(firstElement);
            while (true) {
                if (input[cursor] == ',') {
                    cursor++;
                    JsonValue *element = parseElement();
                    if (element) {
                        elements->push_back(element);
                    }
                } else {
                    break;
                }
            }
        } else {
            cursor = cursorStart;
            parseWs();
        }
        return elements;
    }

    JsonValue *parseElement() {
        parseWs();
        JsonValue *value = parseValue();
        if (value) {
            parseWs();
            return value;
        }
        return NULL;
    }

    void parseWs() {
        while (true) {
            char chr = input[cursor];
            if (chr == ' ' || chr == '\t' || chr == '\n' || chr == 'r') {
                cursor++;
            } else {
                return;
            } 
        }
    }

    JsonString *parseString() {
        int cursorStart = cursor;
        if (input[cursor] == '"') {
            cursor++;
            string *str = parseCharacters();
            if (input[cursor] == '"') {
                cursor++;
                return new JsonString(*str);
            } else {
                throw ParseException("Expected closing quote for a string.");
            }
        }
        return NULL;
    }

    string *parseCharacters() {
        string *characters = new string("");
        while (parseCharacter(characters));
        return characters;
    }

    bool parseCharacter(std::string *characters) {
        char chr = input[cursor];
        if (chr == '"') {
            return false;
        } else if (chr == '\\') {
            cursor++;
            parseEscape(characters);
            return true;
        } else {
            cursor++;
            characters->push_back(chr);
            return true;
        }
    }

    void parseEscape(string *characters) {
        char chr = input[cursor];
        switch (chr) {
            case '"':
            case '\\':
            case '/':
                cursor++;
                characters->push_back(chr);
                break;
            case 'b':
                cursor++;
                characters->push_back('\b');
                break;
            case 'f':
                cursor++;
                characters->push_back('\f');
                break;
            case 'n':
                cursor++;
                characters->push_back('\n');
                break;
            case 'r':
                cursor++;
                characters->push_back('\r');
                break;
            case 't':
                cursor++;
                characters->push_back('\t');
                break;
            case 'u':
                cursor++;
                {
                    int h1 = parseHex();
                    int h2 = parseHex();
                    int h3 = parseHex();
                    int h4 = parseHex();
                    if (h1 != -1 && h2 != -1 && h3 != -1 && h4 != -1) {
                        char chr1 = (h1 * 16) + h2;
                        char chr2 = (h3 * 16) + h4;
                        characters->push_back(chr1);
                        characters->push_back(chr2);
                    } else {
                        throw ParseException("Expected 4 hexadecimal numbers for u escape sequence.");
                    }
                }
                break;
            default:
                throw ParseException("Invalid escape sequence.");
        }
    }

    int parseHex() {
        char chr = toupper(input[cursor]);

        if (chr >= 48 && chr <= 57) {        /* 0-9 */
            cursor++;
            return chr - 48;
        } else if (chr >= 65 && chr <= 70) { /* A-F */
            cursor++;
            return 10 + (chr - 65);
        } else {
            return -1;
        }

    }

    JsonNumber *parseNumber() {
        int cursorStart = cursor;
        if (parseInteger()) {
            parseFraction();
            parseExponent();
            double number = stod(input.substr(cursorStart, cursor - cursorStart));
            return new JsonNumber(number);
        } else {
            return NULL;
        }
    }

    bool parseInteger() {
        bool negative = false;
        if (input[cursor] == '-') {
            cursor++;
            negative = true;
        }
        int cursorStart = cursor;
        if (parseOneNine() && parseDigits()) {
            return true;
        } else {
            cursor = cursorStart;
            if (parseDigit()) {
                return true;
            }
        }
        return false;
    }

    bool parseOneNine() {
        char chr = input[cursor];
        if (chr >= 49 && chr <= 57) {
            cursor++;
            return true;
        }
        return false;
    }

    bool parseDigit() {
        char chr = input[cursor];
        if (chr >= 48 && chr <= 57) {
            cursor++;
            return true;
        }
        return false;
    }

    bool parseDigits() {
        int cursorStart = cursor;
        while (parseDigit());
        if (cursorStart == cursor) {
            return false;
        } else {
            return true;
        }
    }

    void parseFraction() {
        if (input[cursor] == '.') {
            cursor++;
            if (parseDigits()) {
                return;
            } else {
                throw ParseException("No digits after decimal point.");
            }
        }
    }

    void parseExponent() {
        char chr = input[cursor];
        if (chr == 'e' || chr == 'E') {
            cursor++;
            parseSign();
            if (parseDigits()) {
                return;
            } else {
                throw ParseException("No digits after exponent.");
            }
        }
    }

    void parseSign() {
        char chr = input[cursor];
        if (chr == '-' || chr == '+') {
            cursor++;
        }
    }
};

int main() {
    vector<string> testCases {
        "12.56", 
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
        "[]",
        "[[[[5]]], []]",
        "true",
        "[true, false]",
        "{}",
        "{ \"name\": \"Bob\" }",
        "{}",
        "{ \"numbers\": [1, 2, 3, 4] }",
        "{ \"numb\ters\": [1, 2, 3, 4] }",
        "\"\\u1234\"",
        //"\\u1234",
        "I really like her"
    };
    JsonPrinter printer;

    for (int i = 0; i < testCases.size(); i++) {
        string input = testCases[i];
        JsonParser parser(input);
        cout << input << " = ";
        try {
            auto result = parser.parse();
            printer.print(result, &cout);
            cout << ", " << parser.cursor << endl;
        } catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    
    
}