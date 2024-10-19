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
    Type_Checker();
    std::shared_ptr<node> ast_root;
    // unique_name => {original_name, types[]}
    //
    std::unordered_map<std::string, symbol_table_entry> v_table;
    std::unordered_map<std::string, symbol_table_entry> f_table;
    std::string check_type(std::shared_ptr<node> n);
    // gets type from tables, will check first
    symbol_table_entry &get_type(std::shared_ptr<node> n);
    // used for testing to throw immediately to catch unexpected circumstances
    bool compare_types(symbol_table_entry lhs, symbol_table_entry rhs, bool testing = false)
    {

        if (lhs.original_name[0] != rhs.original_name[0])
        {
            std::cout << "\nComparing function and variable nodes\n";
            return false;
        }

        for (int i = 0; i < 4; ++i)
        {
            if (lhs.types[i] != rhs.types[i])
            {
                return false;
            }
        }
        return true;
    };
};
#endif
