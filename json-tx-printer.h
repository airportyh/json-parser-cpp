class JsonTxPrinter {
    JsonPrinter jsonPrinter;
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
        } else if (op->type == JsonString || op->type == JsonNumber || op->type == JsonBoolean || op->type == JsonNull) {
            JsonValue *value = static_cast<JsonValue *>(op);
            jsonPrinter.print(value, out, 1);
        } else if (op->type == JsonTxBinaryOpExpression) {
            JsonTxBinaryOpExpressionNode *binOpExpr = static_cast<JsonTxBinaryOpExpressionNode *>(op);
            print(binOpExpr->left, out, level + 1);
            printBinaryOperator(binOpExpr->op, out);
            print(binOpExpr->right, out, level + 1);
        } else {
            throw "blargh";
        }
        if (level == 0) {
            *out << endl;
        }
    }

    void printBinaryOperator(JsonTxBinaryOperator op, ostream *out) {
        switch (op) {
            case JsonTxPlus:
                *out << " + ";
                break;
            case JsonTxMinus:
                *out << " - ";
                break;
            case JsonTxTimes:
                *out << " * ";
                break;
            case JsonTxDivide:
                *out << " / ";
                break;
            case JsonTxGreaterThan:
                *out << " > ";
                break;
            case JsonTxLessThan:
                *out << " < ";
                break;
            case JsonTxGreaterThanOrEqual:
                *out << " >= ";
                break;
            case JsonTxLessThanOrEqual:
                *out << " <= ";
                break;
            case JsonTxEqual:
                *out << " == ";
                break;
            default:
                *out << " (unknown operator) ";
                break;
        }
    }
};