class JsonTxEvaluator {
    public:
    JsonPrinter printer;
    JsonTxPrinter txPrinter;
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
        } else if (op->type == JsonTxBinaryOpExpression) {
            JsonTxBinaryOpExpressionNode *binOpExpr = static_cast<JsonTxBinaryOpExpressionNode *>(op);
            JsonValue *left = evaluate(value, binOpExpr->left);
            JsonValue *right = evaluate(value, binOpExpr->right);
            switch (binOpExpr->op) {
                case JsonTxPlus:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::number(left->number() + right->number());
                    } else if (left->type == JsonString && right->type == JsonString) {
                        return JsonValue::string(left->string() + right->string());
                    } else {
                        return JsonValue::null();
                    }
                case JsonTxMinus:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::number(left->number() - right->number());
                    } else {
                        return JsonValue::null();
                    }
                case JsonTxTimes:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::number(left->number() * right->number());
                    } else {
                        return JsonValue::null();
                    }
                case JsonTxDivide:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::number(left->number() / right->number());
                    } else {
                        return JsonValue::null();
                    }
                case JsonTxGreaterThan:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::boolean(left->number() > right->number());
                    } else if (left->type == JsonString && right->type == JsonString) {
                        return JsonValue::boolean(left->string() > right->string());
                    } else {
                        return JsonValue::null();
                    }
                case JsonTxLessThan:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::boolean(left->number() < right->number());
                    } else if (left->type == JsonString && right->type == JsonString) {
                        return JsonValue::boolean(left->string() < right->string());
                    } else {
                        return JsonValue::null();
                    }
                case JsonTxGreaterThanOrEqual:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::boolean(left->number() >= right->number());
                    } else if (left->type == JsonString && right->type == JsonString) {
                        return JsonValue::boolean(left->string() >= right->string());
                    } else {
                        return JsonValue::null();
                    }
                    break;
                case JsonTxLessThanOrEqual:
                    if (left->type == JsonNumber && right->type == JsonNumber) {
                        return JsonValue::boolean(left->number() <= right->number());
                    } else if (left->type == JsonString && right->type == JsonString) {
                        return JsonValue::boolean(left->string() <= right->string());
                    } else {
                        return JsonValue::null();
                    }
                    break;
                case JsonTxEqual:
                    return JsonValue::boolean(left->equals(right));
                default:
                    throw "Unsupported binary operation";
            }
        } else if (op->type == JsonString || op->type == JsonNumber || op->type == JsonBoolean || op->type == JsonNull) {
            JsonValue *opValue = static_cast<JsonValue *>(op);
            return opValue;
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
