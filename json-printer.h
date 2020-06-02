class JsonPrinter {
    public:
    void print(JsonValue *value, ostream *out, int level = 0, bool initialIndent = false) {
        string indent (level, '\t');
        if (initialIndent) {
            *out << indent;
        }
        
        if (value->type == JsonNumber) {
            *out << to_string(value->number());
        } else if (value->type == JsonString) {
            string str = value->string();
            quote(str);
            *out << str;
        } else if (value->type == JsonNull) {
            *out << "null";
        } else if (value->type == JsonBoolean) {
            if (value->boolean()) {
                *out << "true";
            } else {
                *out << "false";
            }
        } else if (value->type == JsonArray) {
            vector<JsonValue *> *array = value->array();
            if (array->size() == 0) {
                *out << "[]";
                return;
            }
            *out << "[\n";
            for (int i = 0; i < array->size(); i++) {
                JsonValue *value = (*array)[i];
                if (i > 0) {
                    *out << ",\n";
                }
                print(value, out, level + 1, true);
            }
            *out << "\n" << indent << "]";
        } else if (value->type == JsonObject) {
            std::map<string, JsonValue *> *map = value->object();
            auto it = map->begin();
            if (map->size() == 0) {
                *out << "{}";
                return;
            }
            *out << "{\n";
            bool first = true;
            while (it != map->end()) {
                if (first) {
                    first = false;
                } else {
                    *out << ",\n";
                }
                string str = it->first;
                quote(str);
                *out << indent << "\t" << str << ": ";
                print(it->second, out, level + 1, false);
                it++;
            }
            *out << "\n" << indent << "}";
        }
        if (level == 0) {
            *out << "\n";
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