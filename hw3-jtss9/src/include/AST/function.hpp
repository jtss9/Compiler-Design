#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */
                 const char *const p_name,
                 std::vector<DeclNode *> * p_declarations,
                 const char *const p_return_type,
                 CompoundStatementNode *const p_compound_statement);
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char *getNameCString() const {
        return name.c_str();
    }
    const char *getReturnTypeCString() const {
        std::string res = dataType2Str(return_type) + "(";
        if (declarations->size() != 0) {
            for (auto i : *declarations) {
                for(auto j: *i->getIdentifiers()) {
                    res += j->getNameCString();
                    res += ", ";
                }
            }
            res = res.substr(0, res.size() - 2);
        }
        res += ")";
        return res.c_str();
    }

    void setCompoundStatement(CompoundStatementNode *const p_compound_statement) {
        compound_statement = p_compound_statement;
    }

  private:
    // TODO: name, declarations, return type, compound statement
    std::string name;
    std::vector<DeclNode *> *declarations;
    DataType return_type;
    CompoundStatementNode *compound_statement;
};

#endif
