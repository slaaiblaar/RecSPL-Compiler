#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
#include <set>
#include "ast_node.hpp"

#ifndef type_checker_h
#define type_checker_h
class symbol_table
{
public:
    thread_local static long num_counter;
    thread_local static int string_counter;
    symbol_table() {}
    symbol_table(const symbol_table &other)
    {
        for (auto name_it = other.name_map.begin(); name_it != other.name_map.end(); ++name_it)
        {
            name_map[name_it->first] = name_it->second;
        }
        for (auto id_it = other.id_map.begin(); id_it != other.id_map.end(); ++id_it)
        {
            id_map[id_it->first] = id_it->second;
        }
    }
    std::unordered_map<std::string, std::string> name_map;
    std::unordered_map<std::string, std::string> id_map;

    std::string get_id(std::string name)
    {
        if (name_map.find(name) == name_map.end())
        {
            return "";
        }
        return name_map[name];
    }
    std::string get_type(std::string id)
    {
        if (id_map.find(id) == id_map.end())
        {
            return "";
        }
        return id_map[id];
    }
    std::string bind(std::string name, std::string type)
    {
        if (type == "num")
        {
            name_map[name] = fmt::format("num{}", num_counter++);
        }
        else if (type == "string")
        {
            if (string_counter > 90)
            {
                return "";
            }
            name_map[name] = fmt::format("{}$", (char)string_counter++);
        }
        id_map[name_map[name]] = type;
        return name_map[name];
    }
};
typedef struct symbol_table_entry
{
    std::string original_name;
    std::string types[4];
} symbol_table_entry;

class Type_Checker
{
public:
    Type_Checker();
    std::shared_ptr<node> ast_root;
    // unique_name => {original_name, types[]}
    //
    symbol_table v_table;
    symbol_table f_table;
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
