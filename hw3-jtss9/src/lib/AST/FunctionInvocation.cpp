#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char *const p_name,
                                               std::vector<ExpressionNode *> *const p_expressions)
    : ExpressionNode{line, col}, name(p_name), expressions(p_expressions) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (expressions != nullptr) {
        for (auto i : *expressions) {
            i->accept(p_visitor);
        }
    }
}
