#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <memory>
#include <string>
#include <stack>

#include "AST/PType.hpp"

enum class Kind {
    PROGRAM,
    FUNCTION,
    PARAMETER,
    VARIABLE,
    LOOP_VAR,
    CONSTANT
};

class SymbolEntry {
private:
    std::string m_name;
    Kind m_kind;
    PTypeSharedPtr m_type;
    std::string attribute;
    bool has_error = false;
public:
    SymbolEntry(const std::string &p_name, Kind p_kind, const PTypeSharedPtr &p_type, std::string p_attribute, bool p_has_error)
        : m_name(p_name), m_kind(p_kind), m_type(p_type), attribute(p_attribute), has_error(p_has_error) {}
    
    const char* getNameCString() const {
        return m_name.c_str();
    }
    std::string getName() const {
        return m_name;
    }
    const char* getKindCString() const {
        switch (m_kind) {
            case Kind::PROGRAM: return "program";
            case Kind::FUNCTION: return "function";
            case Kind::PARAMETER: return "parameter";
            case Kind::VARIABLE: return "variable";
            case Kind::LOOP_VAR: return "loop_var";
            case Kind::CONSTANT: return "constant";
            default: return "unknown";
        }
    }
    Kind getKind() const {
        return m_kind;
    }
    PTypeSharedPtr getType() const {
        return m_type;
    }
    const char* getAttributeCString() const {
        return attribute.c_str();
    }
    void setKind(Kind p_kind) {
        m_kind = p_kind;
    }
    void setAttribute(const std::string &p_attribute) {
        attribute = p_attribute;
    }
    void setHasError(bool p_has_error) {
        has_error = p_has_error;
    }
    bool hasError() const {
        return has_error;
    }
    bool isValidRef() {
        return m_kind != Kind::PROGRAM && m_kind != Kind::FUNCTION;
    }
};

class SymbolTable {
public:
    SymbolTable() {entries = std::vector<SymbolEntry>();}
    ~SymbolTable() = default;
    void setLevel(int p_level) {
        level = p_level;
    }
    void addSymbol(const SymbolEntry &p_entry) {
        entries.push_back(p_entry);
    }
    bool hasSymbol(const std::string &p_name) const {
        for (const auto &entry : entries) {
            if (entry.getName() == p_name) {
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<SymbolEntry> getSymbol(const std::string &p_name) const {
        for (const auto &entry : entries) {
            if (entry.getName() == p_name) {
                return std::make_shared<SymbolEntry>(entry);
            }
        }
        return nullptr;
    }
    SymbolEntry* getLastSymbol() {
        return entries.empty() ? nullptr : &entries.back();
    }
    void dumpDemarcation(const char chr) {
        for (size_t i = 0; i < 110; ++i) {
            printf("%c", chr);
        }
        puts("");
    }
    void dumpSymbol(void) {
        dumpDemarcation('=');
        printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
        dumpDemarcation('-');
        for (auto &entry : entries)
        {
            printf("%-33s", entry.getNameCString());
            printf("%-11s", entry.getKindCString());
            printf("%d%-10s", this->level, this->level == 0 ? "(global)" : "(local)");
            printf("%-17s", entry.getType()->getPTypeCString());
            printf("%-11s", entry.getAttributeCString());
            puts("");
        }
        dumpDemarcation('-');
      }

private:
    std::vector<SymbolEntry> entries;
    int level;
};

class SymbolManager {
private:
    std::vector <SymbolTable *> tables;
    int current_level;
public:
    SymbolManager() {
        tables = std::vector<SymbolTable *>();
        current_level = 0;
    }
    void pushScope(SymbolTable *p_table) {
        p_table->setLevel(current_level);
        tables.push_back(p_table);
        current_level++;
    }
    void popScope() {
        if (!tables.empty()) {
            tables.pop_back();
            current_level--;
        }
    }
    SymbolTable* getTopTable() {
        if (tables.empty()) {
            return nullptr;
        }
        return tables.back();
    }
    bool hasLoopVar(const std::string &p_name) const {
        for(auto &table : tables) {
            if (table->hasSymbol(p_name)){
                auto symbol = table->getSymbol(p_name);
                if (symbol && symbol->getKind() == Kind::LOOP_VAR) {
                    return true;
                }
            }
        }
        return false;
    }
    std::shared_ptr<SymbolEntry> getSymbol(const std::string &p_name) const {
        for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
            if ((*it)->hasSymbol(p_name)) {
                return (*it)->getSymbol(p_name);
            }
        }
        return nullptr;
    }
    int tableSize() const {
        return static_cast<int>(tables.size());
    }
};

#endif