#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "sema/ErrorPrinter.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    ErrorPrinter m_error_printer{stderr};
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    SymbolManager symbol_manager;
    std::vector<std::string> source;
    bool opt_dump;

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;
    bool has_error = false;
    bool set_const = false;
    bool func_param = false;
    bool check_index = false;
    bool reading = false;
    bool in_procedure = false;
    std::string func_type = "";

    SemanticAnalyzer(char* p_source[], bool p_opt_dump){
        for (int i = 1; p_source[i] != nullptr; i++) {
            source.push_back(p_source[i]);
        }
        opt_dump = p_opt_dump;
    }

    void listSourceError(int line){
        fprintf(stderr, "%s\n", source[line-1].c_str());
    }
    void cursorError(int col){
        for (int i = 0; i < col - 1; i++) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
    }


    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;
};

#endif
