#ifndef AST_UNARY_OPERATOR_NODE_H
#define AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/operator.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>

class UnaryOperatorNode final : public ExpressionNode {
  private:
    Operator m_op;
    std::unique_ptr<ExpressionNode> m_operand;

  public:
    ~UnaryOperatorNode() = default;
    UnaryOperatorNode(const uint32_t line, const uint32_t col, Operator op,
                      ExpressionNode *p_operand)
        : ExpressionNode{line, col}, m_op(op), m_operand(p_operand) {}

    const char *getOpCString() const {
        return kOpString[static_cast<size_t>(m_op)];
    }

    PTypeSharedPtr getOperandType() const {
        return m_operand->getType();
    }
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
