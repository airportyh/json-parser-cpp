#include <map>
#include <vector>
#include <string>

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