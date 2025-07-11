#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      /* TODO: constant value */
                      const char *const p_constant_type,
                      const char *const p_constant_value);
    ~ConstantValueNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    const char *getConstantValueCString() const {
        if (type == DataType::REAL){
            return std::to_string(std::stof(constant_value)).c_str();
        }
        return constant_value.c_str();
    }

  private:
    // TODO: constant value
    DataType type;
    std::string constant_value;
};

#endif
