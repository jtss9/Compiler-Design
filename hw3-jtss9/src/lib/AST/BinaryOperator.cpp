#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, 
                                       const char *const p_operator,
                                       ExpressionNode *const p_left,
                                       ExpressionNode *const p_right)
    : ExpressionNode{line, col}, op(p_operator), left(p_left), right(p_right) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    left->accept(p_visitor);
    right->accept(p_visitor);
}
