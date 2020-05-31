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
            bool first = true;
            while (it != map->end()) {
                if (first) {
                    first = false;
                } else {
                    *out << ", ";
                }
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