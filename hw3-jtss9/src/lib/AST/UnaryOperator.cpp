#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, 
                                     const char *const p_operator,
                                     ExpressionNode *const p_expression)
    : ExpressionNode{line, col}, op(p_operator), expression(p_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (expression != nullptr) {
        expression->accept(p_visitor);
    }
}
