#include "AST/CompoundStatement.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/program.hpp"
#include "codegen/CodeGenerator.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <cstring>
#include <unordered_map>
#include <utility>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             std::unordered_map<SemanticAnalyzer::AstNodeAddr,
                                                      SymbolManager::Table>
                                 &&p_symbol_table_of_scoping_nodes)
    : m_symbol_manager(false /* no dump */),
      m_source_file_path(source_file_name),
      m_symbol_table_of_scoping_nodes(std::move(p_symbol_table_of_scoping_nodes)) {
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind('/');
    auto dot_pos = source_file_name.rfind('.');

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n"
        ".section    .text\n"
        "    .align 2\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the scope for looking up the symbol entry.
    // Hint: Use m_symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_program)));

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    constexpr const char *const main_function_prologue = 
        "    .globl main\n"
        "    .type main, @function\n"
        "main:\n"
        "    addi sp, sp, -128\n"
        "    sw ra, 124(sp)\n"
        "    sw s0, 120(sp)\n"
        "    addi s0, sp, 128\n";
    dumpInstructions(m_output_file.get(), main_function_prologue);
    m_offset = -12;

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    constexpr const char *const main_function_epilogue =
        "    lw ra, 124(sp)\n"
        "    lw s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr ra\n"
        "    .size main, .-main\n";
    dumpInstructions(m_output_file.get(), main_function_epilogue);

    m_symbol_manager.popScope();

    for(auto& p:m_strings){
        const char *string_instruction =
            ".section .rodata\n"
            "    .align 2\n"
            "%s:\n"
            "    .string \"%s\"\n";
        dumpInstructions(m_output_file.get(), string_instruction,
                            p.first.c_str(), p.second.c_str());
    }

    for(auto &p:m_reals){
        const char *real_instruction =
            ".section .rodata\n"
            "    .align 2\n"
            "%s:\n"
            "    .float %s\n";
        dumpInstructions(m_output_file.get(), real_instruction,
                            p.first.c_str(), p.second.c_str());
    }
}

void CodeGenerator::visit(DeclNode &p_decl) { p_decl.visitChildNodes(*this); }

void CodeGenerator::visit(VariableNode &p_variable) {
    bool has_constant = p_variable.getConstantPtr() != nullptr;
    if (m_symbol_manager.lookup(p_variable.getName())->getLevel() == 0) {
        // Global variable
        if (!has_constant) {
            dumpInstructions(
                m_output_file.get(),
                ".comm %s, 4, 4\n",
                p_variable.getNameCString());
        } else {
            // Global constant
            const char *constant_instruction = 
                ".section .rodata\n"
                "    .align 2\n"
                "    .globl %s\n"
                "    .type %s, @object\n"
                "%s:\n"
                "    .word %s\n";
            dumpInstructions(
                m_output_file.get(), constant_instruction,
                p_variable.getNameCString(),
                p_variable.getNameCString(),
                p_variable.getNameCString(),
                p_variable.getConstantPtr()->getConstantValueCString());
        }
    } else {
        SymbolEntry* symbol = m_symbol_manager.lookup(p_variable.getName());
        symbol->setOffset(m_offset);
        if (has_constant) {
            PTypeSharedPtr constant_type = p_variable.getConstantPtr()->getTypeSharedPtr();
            if (constant_type->isPrimitiveInteger()) {
                const char* assign_instr = 
                    "    addi t0, s0, %d\n"
                    "    li t1, %s\n"
                    "    sw t1, 0(t0)\n";
                dumpInstructions(m_output_file.get(), assign_instr, m_offset,
                                    p_variable.getConstantPtr()->getConstantValueCString());
            } else if (constant_type->isPrimitiveBool()) {
                bool val = strcmp(p_variable.getConstantPtr()->getConstantValueCString(), "true") == 0;
                const char* assign_instr =
                    "    addi t0, s0, %d\n"
                    "    li t1, %d\n"
                    "    sw t1, 0(t0)\n";
                dumpInstructions(m_output_file.get(), assign_instr, m_offset, val);
            }
        } else if (m_function_para) {
            bool is_scalar = p_variable.getTypePtr()->isScalar();
            int d = (m_offset + 12) / (-4);
            if (is_scalar) {
                std::string load_instr;
                if (d < 8) {
                    load_instr = "    sw a%d, %d(s0)\n";
                } else {
                    load_instr = "    sw t%d, %d(s0)\n";
                    d -= 8;
                }
                dumpInstructions(m_output_file.get(), load_instr.c_str(), d, m_offset);
            } else {
                auto dims = p_variable.getTypePtr()->getDimensions();
                int size = 1;
                for (auto &dim : dims) size *= dim;
                std::string lw_instr;
                if (d >= 8) d -= 8;
                for(int i=0; i<size; i++) {
                    if (d < 8) {
                        lw_instr = "    lw t0, %d(a%d)\n";
                    } else {
                        lw_instr = "    lw t0, %d(t%d)\n";
                    }
                    lw_instr += "    sw t0, %d(s0)\n";
                    dumpInstructions(
                        m_output_file.get(),
                        lw_instr.c_str(),
                        i*(-4),
                        d,
                        m_offset + i * (-4)
                    );
                }
            }
        }
        std::vector<uint64_t> dims = p_variable.getTypePtr()->getDimensions();
        int size = 4;
        for (auto dim : dims) {
            size *= dim;
        }
        m_offset -= size;
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    PTypeSharedPtr constantType = p_constant_value.getTypeSharedPtr();
    if (constantType->isPrimitiveInteger()) {
        const char *constant_instruction =
            "    li t0, %s\n"
            "    addi sp, sp, -4\n"
            "    sw t0, 0(sp)\n";
        dumpInstructions(m_output_file.get(), constant_instruction,
                            p_constant_value.getConstantValueCString());
    } else if (constantType->isPrimitiveBool()) {
        const char *constant_instruction =
            "    li t0, %d\n"
            "    addi sp, sp, -4\n"
            "    sw t0, 0(sp)\n";
        bool val = strcmp(p_constant_value.getConstantValueCString(), "true") == 0;
        dumpInstructions(m_output_file.get(), constant_instruction, val);
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_function)));

    // Generate function header
    const char *function_prologue =
        ".section  .text\n"
        "    .align 2\n"
        "    .globl %s\n"
        "    .type  %s, @function\n"
        "%s:\n"
        "    addi sp, sp, -128\n"
        "    sw ra, 124(sp)\n"
        "    sw s0, 120(sp)\n"
        "    addi s0, sp, 128\n";
    dumpInstructions(m_output_file.get(), function_prologue,
                        p_function.getNameCString(),
                        p_function.getNameCString(),
                        p_function.getNameCString());
    
    m_offset = -12;

    m_function_para = true;
    // Generate function parameters
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_function.getParameters().begin(), p_function.getParameters().end(), visit_ast_node);
    m_function_para = false;

    // Generate function body
    p_function.visitBodyChildNodes(*this);

    // Generate function epilogue
    const char *const function_epilogue =
        "    lw ra, 124(sp)\n"
        "    lw s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr ra\n"
        "    .size %s, .-%s\n";
    dumpInstructions(m_output_file.get(), function_epilogue,
                        p_function.getNameCString(),
                        p_function.getNameCString());

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_compound_statement)));

    p_compound_statement.visitChildNodes(*this);
    // for (auto &stmt: p_compound_statement.getStatements()) {
    //     if (m_has_return) break;
    //     stmt->accept(*this);
    // }

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);
    std::string function_name;
    switch(p_print.getTarget().getInferredType()->getPrimitiveType()) {
        case PType::PrimitiveTypeEnum::kIntegerType:
            function_name = "printInt";
            break;
        case PType::PrimitiveTypeEnum::kRealType:
            function_name = "printReal";
            break;
        case PType::PrimitiveTypeEnum::kStringType:
            function_name = "printString";
            break;
        default:
            assert(false && "Unsupported print type");
    }
    if (function_name == "printReal") {
        const char* print_instr =
            "    flw fa0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    jal ra, %s\n";
        dumpInstructions(m_output_file.get(), print_instr, function_name.c_str());
    } else {
        const char* print_instr =
            "    lw a0, 0(sp)\n"
            "    addi sp, sp, 4\n"
            "    jal ra, %s\n";
        dumpInstructions(m_output_file.get(), print_instr, function_name.c_str());
    }
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    std::string binary_instr;
    bool is_real = p_bin_op.getInferredType()->isReal();
    switch(p_bin_op.getOp()){
        case Operator::kPlusOp:
            if(is_real) binary_instr = "    fadd.s ft0, ft0, ft1\n";
            else binary_instr = "    add t0, t0, t1\n";
            break;
        case Operator::kMinusOp:
            if(is_real) binary_instr = "    fsub.s ft0, ft0, ft1\n";
            else binary_instr = "    sub t0, t0, t1\n";
            break;
        case Operator::kMultiplyOp:
            if(is_real) binary_instr = "    fmul.s ft0, ft0, ft1\n";
            else binary_instr = "    mul t0, t0, t1\n";
            break;
        case Operator::kDivideOp:
            if(is_real) binary_instr = "    fdiv.s ft0, ft0, ft1\n";
            else binary_instr = "    div t0, t0, t1\n";
            break;
        case Operator::kModOp:
            binary_instr = "    rem t0, t0, t1\n";
            break;
        case Operator::kAndOp:
            binary_instr = "    and t0, t0, t1\n";
            break;
        case Operator::kOrOp:
            binary_instr = "    or t0, t0, t1\n";
            break;
        case Operator::kLessOp:
            binary_instr = "    slt t0, t0, t1\n";
            break;
        case Operator::kLessOrEqualOp:
            binary_instr = "    slt t0, t1, t0\n"
                           "    xor t0, t0, 1\n";
            break;
        case Operator::kGreaterOp:
            binary_instr = "    slt t0, t1, t0\n";
            break;
        case Operator::kGreaterOrEqualOp:
            binary_instr = "    slt t0, t0, t1\n"
                           "    xor t0, t0, 1\n";
            break;
        case Operator::kEqualOp:
            binary_instr = "    slt t2, t0, t1\n"
                           "    slt t3, t1, t0\n"
                           "    or t0, t2, t3\n"
                           "    xor t0, t0, 1\n";
            break;
        case Operator::kNotEqualOp:
            binary_instr = "    slt t2, t0, t1\n"
                           "    slt t3, t1, t0\n"
                           "    or t0, t2, t3\n";
            break;
        default:
            assert(false && "Unsupported binary operator");
    }
    std::string final_instr;
    if (is_real) {
        final_instr = "    flw ft1, 0(sp)\n"
                      "    addi sp, sp, 4\n"
                      "    flw ft0, 0(sp)\n"
                      "    addi sp, sp, 4\n"
                      "%s"
                      "    addi sp, sp, -4\n"
                      "    fsw ft0, 0(sp)\n";
    } else {
        final_instr = "    lw t1, 0(sp)\n"
                      "    addi sp, sp, 4\n"
                      "    lw t0, 0(sp)\n"
                      "    addi sp, sp, 4\n"
                      "%s"
                      "    addi sp, sp, -4\n"
                      "    sw t0, 0(sp)\n";
    }
    dumpInstructions(m_output_file.get(), final_instr.c_str(), binary_instr.c_str());

}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    std::string unary_instr;
    std::string op;
    switch (p_un_op.getOp()) {
        case Operator::kNegOp:
            unary_instr = "    lw t0, 0(sp)\n"
                          "    addi sp, sp, 4\n"
                          "    sub t0, zero, t0\n"
                          "    addi sp, sp, -4\n"
                          "    sw t0, 0(sp)\n";
            break;
        case Operator::kNotOp:
            unary_instr = "    lw t0, 0(sp)\n"
                          "    addi sp, sp, 4\n"
                          "    xor t0, t0, 1\n"
                          "    addi sp, sp, -4\n"
                          "    sw t0, 0(sp)\n";
            break;
        default:
            assert(false && "Unsupported unary operator");
    }
    dumpInstructions(m_output_file.get(), unary_instr.c_str());
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);
    int arg_count = p_func_invocation.getArguments().size();
    for(int i=arg_count-1; i>=0; i--){
        std::string move_instr;
        if (i>=8) {
            move_instr = "    lw t%d, 0(sp)\n"
                         "    addi sp, sp, 4\n";
            dumpInstructions(m_output_file.get(), move_instr.c_str(), i-8);
        } else {
            move_instr = "    lw a%d, 0(sp)\n"
                         "    addi sp, sp, 4\n";
            dumpInstructions(m_output_file.get(), move_instr.c_str(), i);
        }
    }
    std::string function_name = p_func_invocation.getNameCString();
    const char* call_instr = "    jal ra, %s\n";
    dumpInstructions(m_output_file.get(), call_instr, function_name.c_str());
    SymbolEntry* symbol_entry = m_symbol_manager.lookup(function_name);
    if (!symbol_entry->getTypePtr()->isVoid()) {
        const char* push_instr = "    mv t0, a0\n"
                                 "    addi sp, sp, -4\n"
                                 "    sw t0, 0(sp)\n";
        dumpInstructions(m_output_file.get(), push_instr);
    }
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    SymbolEntry *symbol_entry = m_symbol_manager.lookup(p_variable_ref.getName());
    if (symbol_entry->getLevel() == 0) {
        // Global variable
        const char *load_instr = "    la t0, %s\n";
        dumpInstructions(m_output_file.get(), load_instr,
                            p_variable_ref.getNameCString());
    } else if (symbol_entry->getTypePtr()->isPrimitiveString()){
        if(m_lhs == false){
            const char* get_string = "    lui t0, %%hi(%s)\n"
                                     "    addi t0, t0, %%lo(%s)\n";
            dumpInstructions(m_output_file.get(), get_string,
                                p_variable_ref.getNameCString(),
                                p_variable_ref.getNameCString());
        }
        m_lhs = true;
    } else if (symbol_entry->getTypePtr()->isPrimitiveReal()){
        if (m_lhs == true) {
            const char* get_real =
                "   lui t0, %%hi(%s)\n"
                "   flw ft0, %%lo(%s)(t0)\n"
                "   fsw ft0, %d(s0)\n";
            dumpInstructions(m_output_file.get(), get_real,
                                p_variable_ref.getNameCString(),
                                p_variable_ref.getNameCString(),
                                symbol_entry->getOffset());
            return;
        } else {
            const char *get_real = "    flw ft0, %d(s0)\n";
            dumpInstructions(m_output_file.get(), get_real,
                                symbol_entry->getOffset());
        }
        m_lhs = true;
    } else {
        const char *assign_instr = "    addi t0, s0, %d\n";
        int offset = symbol_entry->getOffset();
        bool is_ref_array = !symbol_entry->getTypePtr()->isScalar();
        if (!symbol_entry->getTypePtr()->isScalar()) {
            auto dims = symbol_entry->getTypePtr()->getDimensions();
            reverse(dims.begin(), dims.end());
            auto &index = p_variable_ref.getIndices();
            int total = 1;
            int size = 0;
            // for(auto it=index.rbegin(); it!=index.rend(); ++it){
            for(int i=index.size()-1, j=dims.size()-1; i>=0 && j>=0; --i, --j){
                is_ref_array = false;
                auto constant = dynamic_cast<ConstantValueNode *>(index[i].get());
                int val = std::stoi(constant->getConstantPtr()->getConstantValueCString());
                size += val * total;
                total *= dims[j];
            }
            offset -= size * 4;
        }
        dumpInstructions(m_output_file.get(), assign_instr, offset);
        if(is_ref_array) m_lhs = true;
    }

    if (!m_lhs){
        const char *load_instr = "    lw t0, 0(t0)\n";
        dumpInstructions(m_output_file.get(), load_instr);
    }
    m_lhs = false;
    const char* push_instr = "    addi sp, sp, -4\n"
                             "    %ssw %st0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), push_instr,
                        (symbol_entry->getTypePtr()->isReal() ? "f" : ""),
                        (symbol_entry->getTypePtr()->isReal() ? "f" : ""));
    
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    bool is_string = m_symbol_manager.lookup(p_assignment.getLvalue().getName())
                         ->getTypePtr()
                         ->isPrimitiveString();
    bool is_real = m_symbol_manager.lookup(p_assignment.getLvalue().getName())
                         ->getTypePtr()
                         ->isPrimitiveReal();
    if (is_string || is_real) {
        auto rval = dynamic_cast<ConstantValueNode *>(&p_assignment.getExpr());
        if (rval && is_real)  {
            m_lhs = true;
            p_assignment.getLvalue().accept(*this);
            m_lhs = false;
        }
        p_assignment.getExpr().accept(*this);
        if (is_string) {
            m_strings.push_back(std::make_pair(p_assignment.getLvalue().getName(),
                                               rval->getConstantPtr()->getConstantValueCString()));
        } else if (is_real) {
            if (rval) {
                m_reals.push_back(std::make_pair(p_assignment.getLvalue().getName(),
                                               rval->getConstantPtr()->getConstantValueCString()));
            } else {
                const char *assign_instr = "    flw ft0, 0(sp)\n"
                                           "    addi sp, sp, 4\n"
                                           "    fsw ft0, %d(s0)\n";
                dumpInstructions(m_output_file.get(), assign_instr,
                                    m_symbol_manager.lookup(p_assignment.getLvalue().getName())->getOffset());
            }
        }
    } else {
        m_lhs = true;
        p_assignment.getLvalue().accept(*this);
        m_lhs = false;
        p_assignment.getExpr().accept(*this);
        const char *assign_instr = "    lw t0, 0(sp)\n"
                                   "    addi sp, sp, 4\n"
                                   "    lw t1, 0(sp)\n"
                                   "    addi sp, sp, 4\n"
                                   "    sw t0, 0(t1)\n";
        dumpInstructions(m_output_file.get(), assign_instr);
    }
}

void CodeGenerator::visit(ReadNode &p_read) {
    m_lhs = true;
    p_read.visitChildNodes(*this);
    m_lhs = false;

    std::string function_name;
    switch(p_read.getTarget().getInferredType()->getPrimitiveType()) {
        case PType::PrimitiveTypeEnum::kIntegerType:
            function_name = "readInt";
            break;
        case PType::PrimitiveTypeEnum::kRealType:
            function_name = "readReal";
            break;
        default:
            assert(false && "Unsupported read type");
    }
    const char *read_instr = "    jal ra, %s\n"
                             "    mv t0, a0\n"
                             "    lw t1, 0(sp)\n"
                             "    addi sp, sp, 4\n"
                             "    sw t0, 0(t1)\n";
    dumpInstructions(m_output_file.get(), read_instr, function_name.c_str());
}

void CodeGenerator::visit(IfNode &p_if) {
    bool has_else = p_if.m_else_body != nullptr;
    std::vector<int> labels = {m_label_num, m_label_num + 1};

    if (has_else) labels.push_back(m_label_num + 2);
    m_label_num += has_else+2;

    p_if.m_condition->accept(*this);
    const char* jump_instr = "    lw t0, 0(sp)\n"
                             "    addi sp, sp, 4\n"
                             "    beq t0, zero, L%d\n";
    dumpInstructions(m_output_file.get(), jump_instr, labels[1]);
    dumpInstructions(m_output_file.get(), "L%d:\n", labels[0]);

    p_if.m_body->accept(*this);
    if (has_else) {
        const char* jump_instr = "    j L%d\n"
                                 "L%d:\n";
        dumpInstructions(m_output_file.get(), jump_instr, labels[2], labels[1]);
        p_if.m_else_body->accept(*this);
        dumpInstructions(m_output_file.get(), "L%d:\n", labels[2]);
    } else {
        dumpInstructions(m_output_file.get(), "L%d:\n", labels[1]);
    }

}

void CodeGenerator::visit(WhileNode &p_while) {
    std::vector<int> labels = {m_label_num, m_label_num + 1};
    m_label_num += 2;
    dumpInstructions(m_output_file.get(), "L%d:\n", labels[0]);
    p_while.m_condition->accept(*this);
    const char* jump_instr = "    lw t0, 0(sp)\n"
                             "    addi sp, sp, 4\n"
                             "    beq t0, zero, L%d\n";
    dumpInstructions(m_output_file.get(), jump_instr, labels[1]);
    p_while.m_body->accept(*this);
    const char *jump_instr2 = "    j L%d\n"
                             "L%d:\n";
    dumpInstructions(m_output_file.get(), jump_instr2, labels[0], labels[1]);
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the scope for looking up the symbol entry.
    std::vector<int> labels = {m_label_num, m_label_num + 1, m_label_num + 2};
    m_label_num += 3;

    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_for)));

    p_for.m_loop_var_decl->accept(*this);
    p_for.m_init_stmt->accept(*this);

    dumpInstructions(m_output_file.get(), "L%d:\n", labels[0]);
    p_for.m_end_condition->accept(*this);

    SymbolEntry *symbol = m_symbol_manager.lookup(p_for.m_loop_var_decl->getVariables()[0]->getName());
    const char *decl_instr = "    addi t0, s0, %d\n"
                             "    lw t0, 0(t0)\n"
                             "    lw t1, 0(sp)\n"
                             "    addi sp, sp, 4\n";
    dumpInstructions(m_output_file.get(), decl_instr, symbol->getOffset());
    const char* exit_loop_instr = "    bge t0, t1, L%d\n"
                                  "L%d:\n";
    dumpInstructions(m_output_file.get(), exit_loop_instr, labels[2], labels[1]);
    p_for.m_body->accept(*this);
    const char *increase_loop_var = "    addi t0, s0, %d\n"
                                    "    lw t1, 0(t0)\n"
                                    "    addi t1, t1, 1\n"
                                    "    sw t1, 0(t0)\n";
    dumpInstructions(m_output_file.get(), increase_loop_var, symbol->getOffset());
    const char *jump_instr = "    j L%d\n"
                             "L%d:\n";
    dumpInstructions(m_output_file.get(), jump_instr, labels[0], labels[2]);
    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    const char* return_instr = "    lw a0, 0(sp)\n"
                               "    addi sp, sp, 4\n";
    dumpInstructions(m_output_file.get(), return_instr);
    m_has_return = true;
}
