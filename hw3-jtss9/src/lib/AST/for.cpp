#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                    DeclNode *const p_declaration,
                    AssignmentNode *const p_assignment,
                    ExpressionNode *const p_expression,
                    CompoundStatementNode *const p_compound_statement)
    : AstNode{line, col}, declaration(p_declaration), assignment(p_assignment),
        expression(p_expression), compound_statement(p_compound_statement) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declaration != nullptr) {
        declaration->accept(p_visitor);
    }
    if(assignment != nullptr) {
        assignment->accept(p_visitor);
    }
    if(expression != nullptr) {
        expression->accept(p_visitor);
    }
    if(compound_statement != nullptr) {
        compound_statement->accept(p_visitor);
    }
}
