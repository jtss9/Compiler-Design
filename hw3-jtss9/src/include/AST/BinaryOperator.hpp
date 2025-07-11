#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       /* TODO: operator, expressions */
                       const char *const p_operator,
                       ExpressionNode *const p_left,
                       ExpressionNode *const p_right);
    ~BinaryOperatorNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    const char *getOperatorCString() const { return op.c_str(); }

  private:
    // TODO: operator, expressions
    std::string op;
    ExpressionNode *left;
    ExpressionNode *right;
};

#endif
