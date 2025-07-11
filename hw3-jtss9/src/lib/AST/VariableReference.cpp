#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char *const p_name)
    : ExpressionNode{line, col}, name(p_name), expressions(nullptr) {}

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char *const p_name,
                                             std::vector<ExpressionNode *> *const p_expressions)
    : ExpressionNode{line, col}, name(p_name), expressions(p_expressions) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (expressions != nullptr) {
        for (auto i : *expressions) {
            i->accept(p_visitor);
        }
    }
}
