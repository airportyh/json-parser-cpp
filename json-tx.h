enum JsonTxType {
    JsonTxPathExpression = 100,       // [0].partner[name].first
    JsonTxCallExpression,             // map(name)
    JsonTxPipeExpression,             // team.people | filter(isCool) | map(name)
    JsonTxBinaryOpExpression,         // filter(age > 3) | map(name)
    JsonTxConditionExpression,        // map(age > 3 ? name + "3+" : name + "toddler")
};

enum JsonTxBinaryOperator {
    JsonTxPlus = 1,
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
