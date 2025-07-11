#include "sema/Error.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include <cstdint>

#define tab fprintf(stderr, "    ");

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    SymbolTable *symbol_table = new SymbolTable();
    symbol_table->addSymbol(
        SymbolEntry(p_program.getNameCString(), Kind::PROGRAM, p_program.getRetType(), "", false)
    );
    symbol_manager.pushScope(symbol_table);
    p_program.visitChildNodes(*this);
    if (opt_dump) {
        symbol_table->dumpSymbol();
    }
    symbol_manager.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto current_table = symbol_manager.getTopTable();
    if (current_table->hasSymbol(p_variable.getNameCString())){
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: symbol '%s' is redeclared\n",
                p_variable.getLocation().line, p_variable.getLocation().col, p_variable.getNameCString());
        tab; listSourceError(p_variable.getLocation().line);
        tab; cursorError(p_variable.getLocation().col);
        return;
    }
    if (symbol_manager.hasLoopVar(p_variable.getNameCString())) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: symbol '%s' is redeclared\n",
                p_variable.getLocation().line, p_variable.getLocation().col, p_variable.getNameCString());
        tab; listSourceError(p_variable.getLocation().line);
        tab; cursorError(p_variable.getLocation().col);
        return;
    }
    Kind var_kind = this->func_param ? Kind::PARAMETER : Kind::VARIABLE;
    current_table->addSymbol(
        SymbolEntry(p_variable.getNameCString(), var_kind, p_variable.getType(), "", false)
    );
    if (p_variable.getType()->isValidArray() == false) {
        has_error = true;
        current_table->getLastSymbol()->setHasError(true);
        fprintf(stderr, "<Error> Found in line %d, column %d: '%s' declared as an array with an index that is not greater than 0\n",
                p_variable.getLocation().line, p_variable.getLocation().col, p_variable.getNameCString());
        tab; listSourceError(p_variable.getLocation().line);
        tab; cursorError(p_variable.getLocation().col);
        return;
    }
    set_const = true;
    p_variable.visitChildNodes(*this);
    set_const = false;
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if (set_const){
        auto current_table = symbol_manager.getTopTable();
        auto last_symbol = current_table->getLastSymbol();
        last_symbol->setKind(Kind::CONSTANT);
        last_symbol->setAttribute(p_constant_value.getConstantValueCString());
    }
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto current_table = symbol_manager.getTopTable();
    if (current_table->hasSymbol(p_function.getNameCString())) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: symbol '%s' is redeclared\n",
                p_function.getLocation().line, p_function.getLocation().col, p_function.getNameCString());
        tab; listSourceError(p_function.getLocation().line);
        tab; cursorError(p_function.getLocation().col);
    }else{
        current_table->addSymbol(
            SymbolEntry(p_function.getNameCString(), Kind::FUNCTION, p_function.getRetType(), p_function.getParameterCString(), false)
        );
    }
    SymbolTable *func_table = new SymbolTable();
    symbol_manager.pushScope(func_table);
    this->func_param = true;
    func_type = p_function.getRetTypeCString();
    if(func_type == "void") in_procedure = true;
    p_function.visitChildNodes(*this);
    in_procedure = false;
    func_type = "";
    if (opt_dump) {
        func_table->dumpSymbol();
    }
    symbol_manager.popScope();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if (this->func_param){
        this->func_param = false;
        p_compound_statement.visitChildNodes(*this);
    } else {
        SymbolTable *symbol_table = new SymbolTable();
        symbol_manager.pushScope(symbol_table);
        p_compound_statement.visitChildNodes(*this);
        if (opt_dump) {
            symbol_table->dumpSymbol();
        }
        symbol_manager.popScope();
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_print.visitChildNodes(*this);
    if(p_print.getTargetType() == nullptr) return;
    std::string type = p_print.getTargetType()->getPTypeCString();
    if (type != "integer" && type != "real" && type != "boolean" && type != "string") {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: expression of print statement must be scalar type\n",
                p_print.getTarget()->getLocation().line, p_print.getTarget()->getLocation().col);
        tab; listSourceError(p_print.getTarget()->getLocation().line);
        tab; cursorError(p_print.getTarget()->getLocation().col);
    }
}
void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_bin_op.visitChildNodes(*this);
    if (p_bin_op.getLeftType() == nullptr || p_bin_op.getRightType() == nullptr) {
        return;
    }
    std::string op = p_bin_op.getOpCString();
    std::string ltype = p_bin_op.getLeftType()->getPTypeCString();
    std::string rtype = p_bin_op.getRightType()->getPTypeCString();
    

    if (op == "+" || op == "-" || op == "*" || op == "/") {
        if (op == "+" && ltype == "string" && rtype == "string") {
            p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kStringType)));
            return;
        }
        if ((ltype != "integer" && ltype != "real") || (rtype != "integer" && rtype != "real")) {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operands to binary operator '%s' ('%s' and '%s')\n",
                    p_bin_op.getLocation().line, p_bin_op.getLocation().col, op.c_str(), ltype.c_str(), rtype.c_str());
            tab; listSourceError(p_bin_op.getLocation().line);
            tab; cursorError(p_bin_op.getLocation().col);
            return;
        } else {
            if (rtype == "integer" && ltype == "integer") {
                p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kIntegerType)));
            } else {
                p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kRealType)));
            }
        }
    } else if (op == "=" || op == "<>" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        if ((ltype =="real" || ltype == "integer") && (rtype == "real" || rtype == "integer")) {
            p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kBoolType)));
        } else {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operands to binary operator '%s' ('%s' and '%s')\n",
                    p_bin_op.getLocation().line, p_bin_op.getLocation().col, op.c_str(), ltype.c_str(), rtype.c_str());
            tab; listSourceError(p_bin_op.getLocation().line);
            tab; cursorError(p_bin_op.getLocation().col);
            return;
        }
    } else if (op == "and" || op == "or") {
        if (ltype != "boolean" || rtype != "boolean") {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operands to binary operator '%s' ('%s' and '%s')\n",
                    p_bin_op.getLocation().line, p_bin_op.getLocation().col, op.c_str(), ltype.c_str(), rtype.c_str());
            tab; listSourceError(p_bin_op.getLocation().line);
            tab; cursorError(p_bin_op.getLocation().col);
            return;
        }
        p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kBoolType)));
    } else if (op == "mod") {
        if (ltype != "integer" || rtype != "integer") {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operands to binary operator '%s' ('%s' and '%s')\n",
                    p_bin_op.getLocation().line, p_bin_op.getLocation().col, op.c_str(), ltype.c_str(), rtype.c_str());
            tab; listSourceError(p_bin_op.getLocation().line);
            tab; cursorError(p_bin_op.getLocation().col);
            return;
        }
        p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kIntegerType)));
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_un_op.visitChildNodes(*this);
    if (p_un_op.getOperandType() == nullptr) {
        return;
    }
    std::string op = p_un_op.getOpCString();
    std::string type = p_un_op.getOperandType()->getPTypeCString();
    if (op == "neg"){
        if (type != "integer" && type != "real") {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operand to unary operator '%s' ('%s')\n",
                    p_un_op.getLocation().line, p_un_op.getLocation().col, op.c_str(), type.c_str());
            tab; listSourceError(p_un_op.getLocation().line);
            tab; cursorError(p_un_op.getLocation().col);
            return;
        }
        p_un_op.setType(p_un_op.getOperandType());
    } else if (op == "not") {
        if (type != "boolean") {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: invalid operand to unary operator '%s' ('%s')\n",
                    p_un_op.getLocation().line, p_un_op.getLocation().col, op.c_str(), type.c_str());
            tab; listSourceError(p_un_op.getLocation().line);
            tab; cursorError(p_un_op.getLocation().col);
            return;
        }
        p_un_op.setType(p_un_op.getOperandType());
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto symbol = symbol_manager.getSymbol(p_func_invocation.getNameCString());
    p_func_invocation.visitChildNodes(*this);
    // printf("func name: %s\n", p_func_invocation.getNameCString());
    if (symbol != nullptr){
        if (symbol->getKind() == Kind::FUNCTION) {
            std::string args = symbol->getAttributeCString();
            std::vector<std::string> arg_types;
            std::string arg;
            for (char c : args) {
                if (c == ',') {
                    while (arg.back() == ' ') {
                        arg.pop_back(); // Remove trailing spaces
                    }
                    while (arg.front() == ' ') {
                        arg.erase(arg.begin()); // Remove leading spaces
                    }
                    arg_types.push_back(arg);
                    arg = "";
                } else {
                    arg += c;
                }
            }
            if (arg != "") {
                while (arg.back() == ' ') {
                    arg.pop_back(); // Remove trailing spaces
                }
                while (arg.front() == ' ') {
                    arg.erase(arg.begin()); // Remove leading spaces
                }
                arg_types.push_back(arg);
            }
            // printf("arg_types: ");
            // for (const auto &arg_type : arg_types) {
            //     printf("%s ", arg_type.c_str());
            // }
            // printf("\n");
            if (arg_types.size() != p_func_invocation.getArgs().size()) {
                has_error = true;
                fprintf(stderr, "<Error> Found in line %d, column %d: too few/much arguments provided for function '%s'\n",
                        p_func_invocation.getLocation().line, p_func_invocation.getLocation().col,
                        p_func_invocation.getNameCString());
                tab; listSourceError(p_func_invocation.getLocation().line);
                tab; cursorError(p_func_invocation.getLocation().col);
                return;
            }
            int args_size = arg_types.size();
            for (int i=0; i<args_size; i++) {
                if (p_func_invocation.getArgs()[i]->getType() == nullptr) {
                    return;
                }
                std::string func_invocation_type = p_func_invocation.getArgs()[i]->getType()->getPTypeCString();
                if (arg_types[i] != func_invocation_type) {
                    if (arg_types[i] == "real" && func_invocation_type == "integer") continue;
                    has_error = true;
                    fprintf(stderr, "<Error> Found in line %d, column %d: incompatible type passing '%s' to parameter of type '%s'\n",
                            p_func_invocation.getArgs()[i]->getLocation().line, p_func_invocation.getArgs()[i]->getLocation().col,
                            func_invocation_type.c_str(), arg_types[i].c_str());
                    tab; listSourceError(p_func_invocation.getArgs()[i]->getLocation().line);
                    tab; cursorError(p_func_invocation.getArgs()[i]->getLocation().col);
                    return;
                }
            }
            p_func_invocation.setType(symbol->getType());
        } else {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: call of non-function symbol '%s'\n",
                    p_func_invocation.getLocation().line, p_func_invocation.getLocation().col,
                    p_func_invocation.getNameCString());
            tab; listSourceError(p_func_invocation.getLocation().line);
            tab; cursorError(p_func_invocation.getLocation().col);
        }
    } else {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: use of undeclared symbol '%s'\n",
                p_func_invocation.getLocation().line, p_func_invocation.getLocation().col,
                p_func_invocation.getNameCString());
        tab; listSourceError(p_func_invocation.getLocation().line);
        tab; cursorError(p_func_invocation.getLocation().col);
        return;
    }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto symbol = symbol_manager.getSymbol(p_variable_ref.getNameCString());
    if (symbol == nullptr) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: use of undeclared symbol '%s'\n",
                p_variable_ref.getLocation().line, p_variable_ref.getLocation().col,
                p_variable_ref.getNameCString());
        tab; listSourceError(p_variable_ref.getLocation().line);
        tab; cursorError(p_variable_ref.getLocation().col);
    } else if (symbol->getKind() == Kind::FUNCTION || symbol->getKind() == Kind::PROGRAM) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: use of non-variable symbol '%s'\n",
                p_variable_ref.getLocation().line, p_variable_ref.getLocation().col,
                p_variable_ref.getNameCString());
        tab; listSourceError(p_variable_ref.getLocation().line);
        tab; cursorError(p_variable_ref.getLocation().col);
    } else if (symbol->hasError() == false) {
        p_variable_ref.visitChildNodes(*this);
        if (reading) {
            if (symbol->getKind() == Kind::CONSTANT || symbol->getKind() == Kind::LOOP_VAR){
                has_error = true;
                fprintf(stderr, "<Error> Found in line %d, column %d: variable reference of read statement cannot be a constant or loop variable\n",
                        p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
                tab; listSourceError(p_variable_ref.getLocation().line);
                tab; cursorError(p_variable_ref.getLocation().col);
            }
        }
        for(auto &expr: p_variable_ref.getIndices()){
            if (expr->getType()== nullptr) return;
            std::string type = expr->getType()->getPTypeCString();
            if (type != "integer") {
                has_error = true;
                fprintf(stderr, "<Error> Found in line %d, column %d: index of array reference must be an integer\n",
                        expr->getLocation().line, expr->getLocation().col);
                tab; listSourceError(expr->getLocation().line);
                tab; cursorError(expr->getLocation().col);
                return;
            }
        }
        if (p_variable_ref.getDimensionsCount() > symbol->getType()->getDimensionCount()) {
            has_error = true;
            fprintf(stderr, "<Error> Found in line %d, column %d: there is an over array subscript on '%s'\n",
                    p_variable_ref.getLocation().line, p_variable_ref.getLocation().col,
                    p_variable_ref.getNameCString());
            tab; listSourceError(p_variable_ref.getLocation().line);
            tab; cursorError(p_variable_ref.getLocation().col);
            return;
        }
        // printf("%s %d\n", p_variable_ref.getNameCString(), p_variable_ref.getDimensionsCount());
        PTypeSharedPtr type = std::shared_ptr<PType>(new PType(symbol->getType()->getPrimitiveType()));
        std::vector<uint64_t>* new_dims = new std::vector<uint64_t>();
        for(auto it=symbol->getType()->getDimensions().begin()+p_variable_ref.getDimensionsCount();
            it!=symbol->getType()->getDimensions().end(); it++) {
            new_dims->push_back(*it);
        }
        // printf("new_dims size: %d\n", new_dims->size());
        type->setDimensions(*new_dims);
        p_variable_ref.setType(type);
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_assignment.visitChildNodes(*this);
    if (p_assignment.getLvalueType() == nullptr) return;
    std::string ltype = p_assignment.getLvalueType()->getPTypeCString();
    if (ltype != "integer" && ltype != "real" && ltype != "boolean" && ltype != "string") {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: array assignment is not allowed\n",
                p_assignment.getLvalue()->getLocation().line, p_assignment.getLvalue()->getLocation().col);
        tab; listSourceError(p_assignment.getLvalue()->getLocation().line);
        tab; cursorError(p_assignment.getLvalue()->getLocation().col);
        return;
    }
    std::string lval_name = p_assignment.getLvalue()->getNameCString();
    auto lval_symbol = symbol_manager.getSymbol(lval_name);
    if(lval_symbol->getKind() == Kind::CONSTANT) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: cannot assign to variable '%s' which is a constant\n",
                p_assignment.getLvalue()->getLocation().line, p_assignment.getLvalue()->getLocation().col, lval_name.c_str());
        tab; listSourceError(p_assignment.getLvalue()->getLocation().line);
        tab; cursorError(p_assignment.getLvalue()->getLocation().col);
        return;
    }
    if (lval_symbol->getKind() == Kind::LOOP_VAR) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: the value of loop variable cannot be modified inside the loop body\n",
                p_assignment.getLvalue()->getLocation().line, p_assignment.getLvalue()->getLocation().col);
        tab; listSourceError(p_assignment.getLvalue()->getLocation().line);
        tab; cursorError(p_assignment.getLvalue()->getLocation().col);
        return;
    }
    if (p_assignment.getExprType() == nullptr) return;
    std::string expr_type = p_assignment.getExprType()->getPTypeCString();
    if (expr_type != "integer" && expr_type != "real" && expr_type != "boolean" && expr_type != "string") {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: array assignment is not allowed\n",
                p_assignment.getExpr().getLocation().line, p_assignment.getExpr().getLocation().col);
        tab; listSourceError(p_assignment.getExpr().getLocation().line);
        tab; cursorError(p_assignment.getExpr().getLocation().col);
        return;
    }
    if (ltype != expr_type) {
        if((ltype == "integer" && expr_type == "real") || (ltype == "real" && expr_type == "integer")) return;
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: assigning to '%s' from incompatible type '%s'\n",
                p_assignment.getLocation().line, p_assignment.getLocation().col,
                ltype.c_str(), expr_type.c_str());
        tab; listSourceError(p_assignment.getLocation().line);
        tab; cursorError(p_assignment.getLocation().col);
        return;
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    reading = true;
    p_read.visitChildNodes(*this);
    reading = false;

    if (p_read.getTargetType() == nullptr) return;
    std::string type = p_read.getTargetType()->getPTypeCString();
    if (type == "integer" || type == "real" || type == "boolean" || type == "string") {
        return;
    } else {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: variable reference of read statement must be scalar type\n",
                p_read.getTarget().getLocation().line, p_read.getTarget().getLocation().col);
        tab; listSourceError(p_read.getTarget().getLocation().line);
        tab; cursorError(p_read.getTarget().getLocation().col);
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_if.visitChildNodes(*this);
    if (p_if.getConditionType() == nullptr) return;
    std::string cond_type = p_if.getConditionType()->getPTypeCString();
    if (cond_type != "boolean") {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: the expression of condition must be boolean type\n",
                p_if.getCondition()->getLocation().line, p_if.getCondition()->getLocation().col);
        tab; listSourceError(p_if.getCondition()->getLocation().line);
        tab; cursorError(p_if.getCondition()->getLocation().col);
        return;
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_while.visitChildNodes(*this);
    if (p_while.getConditionType() == nullptr) return;
    std::string cond_type = p_while.getConditionType()->getPTypeCString();
    if (cond_type != "boolean") {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: the expression of condition must be boolean type\n",
                p_while.getCondition()->getLocation().line, p_while.getCondition()->getLocation().col);
        tab; listSourceError(p_while.getCondition()->getLocation().line);
        tab; cursorError(p_while.getCondition()->getLocation().col);
        return;
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    SymbolTable *for_table = new SymbolTable();
    symbol_manager.pushScope(for_table);
    auto &loop_var = p_for.m_loop_var_decl->getVariables()[0];
    if (symbol_manager.hasLoopVar(loop_var->getNameCString())) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: symbol '%s' is redeclared\n",
                loop_var->getLocation().line, loop_var->getLocation().col, loop_var->getNameCString());
        tab; listSourceError(loop_var->getLocation().line);
        tab; cursorError(loop_var->getLocation().col);
    } else {
        for_table->addSymbol(
            SymbolEntry(loop_var->getNameCString(), Kind::LOOP_VAR, loop_var->getType(), "", false)
        );
    }

    ConstantValueNode *begin = dynamic_cast<ConstantValueNode *>(&p_for.m_init_stmt->getExpr());
    ConstantValueNode *end = dynamic_cast<ConstantValueNode *>(&(*p_for.m_end_condition));
    int begin_val = std::stoi(begin->getConstantValueCString());
    int end_val = std::stoi(end->getConstantValueCString());
    if (begin_val > end_val) {
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: the lower bound and upper bound of iteration count must be in the incremental order\n",
                p_for.getLocation().line, p_for.getLocation().col);
        tab; listSourceError(p_for.getLocation().line);
        tab; cursorError(p_for.getLocation().col);
    }
    p_for.m_body->accept(*this);
    if (opt_dump) {
        for_table->dumpSymbol();
    }
    symbol_manager.popScope();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if (func_type == ""){
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: program/procedure should not return a value\n",
                p_return.getLocation().line, p_return.getLocation().col);
        tab; listSourceError(p_return.getLocation().line);
        tab; cursorError(p_return.getLocation().col);
        return;
    }
    p_return.visitChildNodes(*this);
    if (p_return.getReturnType() == nullptr) return;
    std::string return_type = p_return.getReturnType()->getPTypeCString();
    if (return_type != func_type) {
        if (return_type == "integer" && func_type == "real") return;
        has_error = true;
        fprintf(stderr, "<Error> Found in line %d, column %d: return '%s' from a function with return type '%s'\n",
                p_return.getReturnValue()->getLocation().line, p_return.getReturnValue()->getLocation().col,
                return_type.c_str(), func_type.c_str());
        tab; listSourceError(p_return.getReturnValue()->getLocation().line);
        tab; cursorError(p_return.getReturnValue()->getLocation().col);
        return;
    }
}
