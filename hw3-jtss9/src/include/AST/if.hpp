#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
           /* TODO: expression, compound statement, compound statement */
           ExpressionNode *const p_expression,
           CompoundStatementNode *const p_compound_statement,
           CompoundStatementNode *const p_else_compound_statement);
    ~IfNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: expression, compound statement, compound statement
    ExpressionNode *expression;
    CompoundStatementNode *compound_statement;
    CompoundStatementNode *else_compound_statement;
};

#endif
