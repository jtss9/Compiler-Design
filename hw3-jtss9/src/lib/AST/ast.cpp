#include <AST/ast.hpp>
#include <cstring>

// prevent the linker from complaining
DataType str2DataType(const char *const p_str) {
    if (p_str == nullptr) {
        return DataType::UNDEFINED;
    }
    if (strcmp(p_str, "int") == 0) {
        return DataType::INT;
    } else if (strcmp(p_str, "real") == 0) {
        return DataType::REAL;
    } else if (strcmp(p_str, "bool") == 0) {
        return DataType::BOOL;
    } else if (strcmp(p_str, "string") == 0) {
        return DataType::STRING;
    } else if (strcmp(p_str, "void") == 0) {
        return DataType::VOID;
    }
    return DataType::UNDEFINED;
}

std::string dataType2Str(const DataType p_type) {
    switch (p_type) {
        case DataType::INT:
            return "int";
        case DataType::REAL:
            return "real";
        case DataType::BOOL:
            return "bool";
        case DataType::STRING:
            return "string";
        case DataType::VOID:
            return "void";
        default:
            return "undefined";
    }
}

AstNode::~AstNode() {}

AstNode::AstNode(const uint32_t line, const uint32_t col)
    : location(line, col) {}

const Location &AstNode::getLocation() const { return location; }
