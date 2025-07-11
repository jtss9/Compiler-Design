#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "PType.hpp"

class ExpressionNode : public AstNode {
  public:
    ~ExpressionNode() = default;
    ExpressionNode(const uint32_t line, const uint32_t col, const PTypeSharedPtr &p_type = nullptr)
        : AstNode{line, col}, m_type(p_type) {}

    const PTypeSharedPtr &getType() const {
        return m_type;
    }
    void setType(const PTypeSharedPtr &p_type) {
        m_type = p_type;
    }
  protected:
    // for carrying type of result of an expression
    PTypeSharedPtr m_type;
    // TODO: for next assignment
};

#endif
