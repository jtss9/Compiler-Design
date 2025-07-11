#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 /* TODO: variable name, type, constant value */
                 const char *const p_name,
                 const char *const p_type,
                 ConstantValueNode *const p_constant_value,
                 std::vector<int> *const p_dims);
    ~VariableNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    const char *getNameCString() const {
        return name.c_str();
    };
    std::string getTypeCString() const {
        std::string res = dataType2Str(type);
        if(dims != nullptr && dims -> size() != 0){
          res += " ";
          for(auto i: *dims){
            res += "[" + std::to_string(i) + "]";
          }
        } 
        return res;
    };
    void setType(DataType p_type) {
        type = p_type;
    }
    void setConstantValue(ConstantValueNode *const p_constant_value) {
        constant_value = p_constant_value;
    }

  private:
    // TODO: variable name, type, constant value
    std::string name;
    DataType type;
    ConstantValueNode *constant_value;
    std::vector<int> *dims;
};

#endif
