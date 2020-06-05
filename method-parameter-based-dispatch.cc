class AstNode {
    virtual int type() = 0;
};

class VarDeclarationNode : public AstNode {
    int type() { return 0; }
};

class BinaryOperationNode : public AstNode {
    int type() { return 1; }
};

template <class Subclass>
class Visitor {
    public:
    virtual void Visit(AstNode *node) {
        if (node->type == 1) {
            Visit
        }
    }
};

class VarDeclarationVisitor : public Visitor<VarDeclarationVisitor> {
    void Visit(AstNode *node);
};

void VarDeclarationVisitor::Visit(AstNode *node) {

}
