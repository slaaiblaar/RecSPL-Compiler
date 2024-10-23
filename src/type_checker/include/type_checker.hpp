#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
#include <set>
#include "ast_node.hpp"

#ifndef type_checker_h
#define type_checker_h
typedef struct symbol_table_entry
{
    std::string original_name;
    std::string types[4];
} symbol_table_entry;

class Type_Checker
{
public:
    std::vector<std::pair<std::string, std::shared_ptr<node>>> type_errors;
    Type_Checker();
    std::shared_ptr<node> ast_root;
    // unique_name => {original_name, types[]}
    //
    std::string type_of(std::shared_ptr<node> n);
    // gets type from tables, will check first
    bool check(std::shared_ptr<node> n);

    bool prog(std::shared_ptr<node> n);
    bool globvars(std::shared_ptr<node> n);
    bool vtyp(std::shared_ptr<node> n);
    bool vname(std::shared_ptr<node> n);
    bool algo(std::shared_ptr<node> n);
    bool instruc(std::shared_ptr<node> n);
    bool command(std::shared_ptr<node> n);
    bool atomic(std::shared_ptr<node> n);
    bool cons(std::shared_ptr<node> n);
    bool assign(std::shared_ptr<node> n);
    bool branch(std::shared_ptr<node> n);
    bool term(std::shared_ptr<node> n);
    bool op(std::shared_ptr<node> n);
    bool arg(std::shared_ptr<node> n);
    bool cond(std::shared_ptr<node> n);
    bool simple(std::shared_ptr<node> n);
    bool composit(std::shared_ptr<node> n);
    bool unop(std::shared_ptr<node> n);
    bool binop(std::shared_ptr<node> n);
    bool fname(std::shared_ptr<node> n);
    bool functions(std::shared_ptr<node> n);
    bool decl(std::shared_ptr<node> n);
    bool header(std::shared_ptr<node> n);
    bool body(std::shared_ptr<node> n);
    bool prolog(std::shared_ptr<node> n);
    bool epilog(std::shared_ptr<node> n);
    bool locvars(std::shared_ptr<node> n);
    bool subfuncs(std::shared_ptr<node> n);
    bool keyword(std::shared_ptr<node> n);
    bool literal(std::shared_ptr<node> n);
    // used for testing to throw immediately to catch unexpected circumstances
    // bool check(std::shared_ptr<node> n);
};
#endif
