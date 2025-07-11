#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *const p_variable_reference)
    : AstNode{line, col}, variable_reference(p_variable_reference) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (variable_reference != nullptr) {
        variable_reference->accept(p_visitor);
    }
}
