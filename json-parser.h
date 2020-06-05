#include <string>

using namespace std;

class JsonParser : public BaseParser {
    public:

    JsonParser(string input): BaseParser(input) {}

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

    JsonValue *parseObject() {
        if (input[cursor] == '{') {
            cursor++;
            int cursorStart = cursor;
            std::map<string, JsonValue *> *map = parseMembers();
            if (map) {
                if (input[cursor] == '}') {
                    cursor++;
                    return JsonValue::object(map);
                } else {
                    throw ParseException("Expected closing bracket for an object.");
                }
            } else {
                cursor = cursorStart;
                parseWs();
                if (input[cursor] == '}') {
                    cursor++;
                    std::map<string, JsonValue *> *map = new std::map<string, JsonValue *>();
                    return JsonValue::object(map);
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
        JsonValue *key = parseString();
        if (key) {
            parseWs();
            if (input[cursor] == ':') {
                cursor++;
                JsonValue *element = parseElement();
                if (element) {
                    (*map)[key->string()] = element;
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
                    return JsonValue::array(elements);
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

};
