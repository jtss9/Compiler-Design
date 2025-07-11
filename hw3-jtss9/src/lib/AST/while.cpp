#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col, 
                     ExpressionNode *const p_condition,
                     CompoundStatementNode *const p_body)
    : AstNode{line, col}, expression(p_condition), compound_statement(p_body) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (expression != nullptr) {
        expression->accept(p_visitor);
    }
    if (compound_statement != nullptr) {
        compound_statement->accept(p_visitor);
    }
}
