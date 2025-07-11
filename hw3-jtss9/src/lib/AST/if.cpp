#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
               ExpressionNode *const p_condition,
               CompoundStatementNode *const p_body,
               CompoundStatementNode *const p_else_body)
    : AstNode{line, col},
      expression(p_condition),
      compound_statement(p_body),
      else_compound_statement(p_else_body) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (expression != nullptr) {
        expression->accept(p_visitor);
    }
    if (compound_statement != nullptr) {
        compound_statement->accept(p_visitor);
    }
    if (else_compound_statement != nullptr) {
        else_compound_statement->accept(p_visitor);
    }
}
