class JsonTxParser: public BaseParser {
    public:
    JsonTxParser(string input): BaseParser(input) {}

    /*
    expr -> pipe
    */
    AstNode * parse() {
        return parsePipe();
    }

    /*
    pipe_expr -> non_pipe_expr
               | non_pipe_expr _ "|" _ pipe_expr
    */
    AstNode * parsePipe() {
        std::vector<AstNode *> *ops = new std::vector<AstNode *>();
        while (true) {
            AstNode *op = parseNonPipe();
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
    non_pipe -> binary_expr
              | condition_expr
              | unary_expr
    */
    AstNode * parseNonPipe() {
        int cursorStart = cursor;
        AstNode *result;
        cursor = cursorStart;
        result = parseBinary();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseUnary();
        if (result) {
            return result;
        }
        return NULL;
        /*
        cursor = cursorStart;
        return parseConditionOperation();*/
    }

    /*
        map_op -> "map(" _ op _ ")"
    */
    AstNode *parseMap() {
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
    AstNode *parseFilter() {
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
    AstNode * parsePath() {
        std::vector<string> *props = new std::vector<string>();
        if (parsePathFirst(props)) {
            while (true) {
                if (!parsePathNonFirst(props)) {
                    break;
                }
            }
            return new JsonTxPathExpressionNode(props);
        } else {
            return NULL;
        }
    }

    bool parsePathFirst(std::vector<string> *props) {
        int cursorStart = cursor;
        string *prop = parseNonNumericKey();
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

    bool parsePathNonFirst(std::vector<string> *props) {
        int cursorStart = cursor;
        if (input[cursor] == '.') {
            cursor++;
            string *prop = parseNonNumericKey();
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
        key -> alphanum
             | alphanum identifier
    */
    string * parseNonNumericKey() {
        string identifier = "";
        // alpha-numeric
        while (true) {
            char chr = input[cursor];
            if ((chr >= 65 && chr <= 90) || (chr >= 97 && chr <= 122)) {
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

    string * parseMaybeNumericKey() {
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
            string * prop = parseMaybeNumericKey();
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

    /*
    unary_expr -> call_expr
                | path_expr
                | string
                | number
                | boolean
                | null
                | "(" expr ")"
                
    */
    AstNode *parseUnary() {
        int cursorStart = cursor;
        AstNode *result;
        result = parseMap();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseFilter();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseString();
        if (result) {
            return result;
        }
        
        cursor = cursorStart;
        result = parseNumber();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseBoolean();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parseNull();
        if (result) {
            return result;
        }
        cursor = cursorStart;
        result = parsePath();
        if (result) {
            return result;
        }
        // TODO: parenthesized complex expressions
        return NULL;
    }

    AstNode *parseBinary() {
        AstNode *left = parseUnary();
        int cursorStart = cursor;
        if (left) {
            parseWs();
            unsigned char opType = parseOperator();
            if (opType) {
                parseWs();
                AstNode *right = parseBinary();
                return new JsonTxBinaryOpExpressionNode(left, (JsonTxBinaryOperator)opType, right);
            }
        }
        cursor = cursorStart;
        return left;
    }

    unsigned char parseOperator() {
        if (input.substr(cursor, 2) == "==") {
            cursor += 2;
            return JsonTxEqual;
        } else {
            return 0;
        }
    }

};
