#ifndef AST_VARIABLE_NODE_H
#define AST_VARIABLE_NODE_H

#include "AST/PType.hpp"
#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <string>

class VariableNode final : public AstNode {
  private:
    std::string m_name;
    PTypeSharedPtr m_type;
    std::shared_ptr<ConstantValueNode> m_constant_value_node_ptr;

  public:
    ~VariableNode() = default;
    VariableNode(const uint32_t line, const uint32_t col,
                 const std::string &p_name, const PTypeSharedPtr &p_type,
                 const std::shared_ptr<ConstantValueNode> &p_constant_value_node)
        : AstNode{line, col}, m_name(p_name), m_type(p_type),
          m_constant_value_node_ptr(p_constant_value_node) {}

    const char *getNameCString() const { return m_name.c_str(); }
    const char *getTypeCString() const { return m_type->getPTypeCString(); }
    
    const PTypeSharedPtr &getType() const { return m_type; }
    bool isConstant() const {
        return m_constant_value_node_ptr != nullptr;
    }
    const std::shared_ptr<ConstantValueNode> &getConstantValueNode() const {
        return m_constant_value_node_ptr;
    }
    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
