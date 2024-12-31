#ifndef __AGGREGATED_AST_PYAST_H__
#define __AGGREGATED_AST_PYAST_H__

#include <filesystem>
#include <charconv>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <tree_sitter/api.h>
#include <tree-sitter-python.h>

#include "./ast.h"


namespace aggregated_ast
{
class PyAst : public Ast
{
public:
    PyAst(const std::filesystem::path& source_path);
    
private:
    class Builder;
};


// Build aggregated AST from complete AST
class PyAst::Builder
{
public:
    Builder(PyAst& ast, const std::string& source);

private:
    struct NodePtr { Node* current, *predecessor, *parent; };

private:
    void walkAST();
    void walkChildren();
    void walkElif();
    void beginNode();
    void endNode() { _node_ptr.predecessor = _node_ptr.current; }

    void addTerminalNode(const TSNode& node, const std::string& type);
    std::string queryName(TSNode node);
    // Update node pointers and return original pointers
    NodePtr updateNodePtr();

private:
    PyAst& _ast;
    const std::string& _source;
    TSTreeCursor* _cursor;
    Node* _root;
    NodePtr _node_ptr;
    
    const std::unordered_map<std::string, decltype(&Builder::walkAST)> _non_terminal_map;
    const std::unordered_set<std::string> _terminal_set;
    const std::unordered_set<std::string> _non_terminal_set;
    const std::unordered_set<std::string> _literal_set;
    const std::unordered_map<std::string, std::string> _addtional_token_map;

private:
    // Block -> function_definition | class_definition
    void block();
    // Statement -> simple statements | decorator
    void statement();

    void for_statement();
    void while_match_statement();
    void if_statement();
    void try_statement();
    void with_statement();
    
    void if_clause();
    // else_clause | finally_clause
    void else_clause();
    // except_clause | except_group_clause
    void except_clause();
    void case_clause();
    void conditional_expression();
    void for_in_clause();
};
}
#endif
