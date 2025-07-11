#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector<VariableNode *> *const p_identifiers,
                   const char *const p_type,
                   std::vector<int> *p_dims)
    : AstNode{line, col}, type(str2DataType(p_type)), dims(p_dims) {
        identifiers = new std::vector<VariableNode *>();
        for (auto &i : *p_identifiers) {
            identifiers->push_back(new VariableNode(
                i->getLocation().line, i->getLocation().col, i->getNameCString(),
                p_type, nullptr, p_dims));
        }
    }

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
                   std::vector<VariableNode *> *const p_identifiers,
                   const char *const p_type,
                   ConstantValueNode *const p_constant_value)
    : AstNode{line, col}, type(str2DataType(p_type)), dims(nullptr) {
        identifiers = new std::vector<VariableNode *>();
        for (auto &i : *p_identifiers) {
            identifiers->push_back(new VariableNode(
                i->getLocation().line, i->getLocation().col, i->getNameCString(),
                p_type, p_constant_value, nullptr));
        }
    }

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (identifiers != nullptr) {
        for (auto i : *identifiers) {
            i->accept(p_visitor);
        }
    }
}
