#include <map>
#include <vector>
#include <string>

using namespace std;

class AstNode {
    public:
    unsigned char type;

    AstNode() {}

    AstNode (unsigned char t): type(t) {}
};

enum JsonValueType { JsonString, JsonNumber, JsonArray, JsonObject, JsonBoolean, JsonNull };

union JsonValueData {
    std::map<std::string, class JsonValue *> *object;
    string string;
    vector<JsonValue *> *array;
    double number;
    bool boolean;

    JsonValueData() {}
    ~JsonValueData() {}
};

class JsonValue : public AstNode {
    public:
    union JsonValueData data;

    JsonValue() {}

    static JsonValue *string(string string) {
        JsonValue *result = new JsonValue();
        result->type = JsonString;
        result->data.string = string;
        return result;
    }

    static JsonValue *number(double number) {
        JsonValue *result = new JsonValue();
        result->type = JsonNumber;
        result->data.number = number;
        return result;
    }

    static JsonValue *object(std::map<std::string, JsonValue *> *object) {
        JsonValue *result = new JsonValue();
        result->type = JsonObject;
        result->data.object = object;
        return result;
    }

    static JsonValue *array(vector<JsonValue *> *array) {
        JsonValue *result = new JsonValue();
        result->type = JsonArray;
        result->data.array = array;
        return result;
    }

    static JsonValue* boolean(bool boolean) {
        JsonValue *result = new JsonValue();
        result->type = JsonBoolean;
        result->data.boolean = boolean;
        return result;
    }

    static JsonValue* null() {
        JsonValue *result = new JsonValue();
        result->type = JsonNull;
        return result;
    }

    std::string string() {
        return data.string;
    }

    std::map<std::string, JsonValue *> *object() {
        return data.object;
    }

    vector<JsonValue *> *array() {
        return data.array;
    }

    double number() {
        return data.number;
    }

    bool boolean() {
        return data.boolean;
    }

    bool isTruthy() {
        if (type == JsonNumber) {
            return number() != 0;
        } else if (type == JsonString) {
            return string().size() != 0;
        } else if (type == JsonArray) {
            return true;
        } else if (type == JsonObject) {
            return true;
        } else if (type == JsonBoolean) {
            return boolean();
        } else if (type == JsonNull) {
            return false;
        } else {
            throw "unknown type";
        }
    }

    bool equals(JsonValue *other) {
        if (type != other->type) {
            return false;
        }
        if (type == JsonNumber) {
            return number() == other->number();
        } else if (type == JsonString) {
            return string() == other->string();
        } else if (type == JsonArray) {
            return array() == other->array();
        } else if (type == JsonObject) {
            return object() == other->object();
        } else if (type == JsonBoolean) {
            return boolean() == other->boolean();
        } else if (type == JsonNull) {
            return true;
        } else {
            throw "Blargh";
        }
    }
};