#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, 
            const uint32_t col,
            std::vector<DeclNode *> *const p_declarations,
            std::vector<AstNode *> *const p_statements)
    : AstNode{line, col}, declarations(p_declarations), statements(p_statements) {}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declarations != nullptr) {
        for(auto i: *declarations) {
            i->accept(p_visitor);
        }
    }
    if(statements != nullptr) {
        for(auto i: *statements) {
            i->accept(p_visitor);
        }
    }
}
