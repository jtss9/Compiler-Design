#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name, const char *const p_return_type,
                         std::vector<DeclNode *> *const p_declarations,
                         std::vector<FunctionNode *> *const p_functions,
                         CompoundStatementNode *const p_body)
    : AstNode{line, col}, name(p_name), return_type(str2DataType(p_return_type)), 
        declarations(p_declarations), functions(p_functions), m_body(p_body) {}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}


void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
    // TODO
    m_body->accept(p_visitor);
    if (declarations != nullptr) {
        for (auto i : *declarations) {
            i->accept(p_visitor);
        }
    }
    if (functions != nullptr) {
        for (auto i : *functions) {
            i->accept(p_visitor);
        }
    }
}
