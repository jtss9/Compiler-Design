#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, 
                           const char *const p_name,
                           const char *const p_type,
                           ConstantValueNode *const p_constant_value,
                           std::vector<int> *const p_dims)
    : AstNode{line, col}, name(p_name), type(str2DataType(p_type)),
      constant_value(p_constant_value), dims(p_dims) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (constant_value != nullptr) {
        constant_value->accept(p_visitor);
    }
}
