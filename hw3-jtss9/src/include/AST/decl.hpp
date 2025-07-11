#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
             /* TODO: identifiers, type */
             std::vector<VariableNode *> *const p_identifiers,
             const char *const p_type,
             std::vector<int> *p_dims);

    // constant variable declaration
    DeclNode(const uint32_t, const uint32_t col,
            /* TODO: identifiers, constant */
            std::vector<VariableNode *> *const p_identifiers,
            const char *const p_type,
            ConstantValueNode *const p_constant_value);

    ~DeclNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    const char *getTypeCString() const {
        std::string res = dataType2Str(type);;
        if(dims != nullptr && dims->size() > 0) {
            res += " ";
            for(auto i: *dims) {
                res += "[";
                res += std::to_string(i);
                res += "]";
            }
        }
        return res.c_str();
    }
    std::vector<VariableNode *> *getIdentifiers() const {
        return identifiers;
    }

  private:
    // TODO: variables
    std::vector<VariableNode *> *identifiers;
    DataType type;
    std::vector<int> *dims;
};

#endif
