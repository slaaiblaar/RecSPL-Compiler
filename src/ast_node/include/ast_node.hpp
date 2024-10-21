#ifndef ast_node_hpp
#define ast_node_hpp

#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
#include <set>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <fmt/core.h>
#include <thread>
// making a type alias because its super annoying to repeatedly define objects
// of type std::shared_ptr<std::unordered_map<std::string, std::string>>
// using sym_table_type = std::unordered_map<std::string, std::string[4]>;
using sym_table_type = std::unordered_map<std::string, std::string>;
class node : public std::enable_shared_from_this<node>
{
public:
    std::string NAME;  // Leaf or Internal node
    int UID;           // Unique identifier
    std::string CLASS; // Class of the node (nonterminal or terminal type)
    std::string WORD;  // Word representing the production
    std::string printnode(int depth, std::string called_from);
    // {v_name: type}
    sym_table_type v_table;
    // {f_name: [returntype, arg1, arg1, arg3]}
    sym_table_type f_table;
    // stores unique v ids like in chapter 6
    std::unordered_map<std::string, std::string> v_id_map;
    // stores unique f ids like in chapter 6
    std::unordered_map<std::string, std::string> f_id_map;
    bool pre_processed = false;
    bool was_printed = false;
    bool is_in_scope = true;
    long node_id;
    int prod = -1;
    int row = -1;
    int col = -1;
    std::string file;
    thread_local static long node_id_counter;
    thread_local static long v_counter;
    thread_local static long f_counter;
    std::shared_ptr<sym_table_type> type_table;
    std::string bind_v(std::string n, std::string t)
    {
        std::string id = fmt::format("{}var{}", t, v_counter++);
        (*type_table)[id] = t;
        v_table[n] = id;
        (*scope_v_table)[n] = id;
        return id;
    }
    std::string bind_f(std::string n, std::string t)
    {
        std::string id = fmt::format("{}fun{}", t, f_counter++);
        (*type_table)[id] = t;
        std::cout << fmt::format("ID({}): {}\n", n, id);
        f_table[n] = id;
        (*scope_f_table)[n] = id;
        return id;
    }
    // essentially stores a list of "siblibgs" for the given types of id
    // is used to detect re-declarations within same scope
    std::shared_ptr<sym_table_type> scope_v_table;
    std::shared_ptr<sym_table_type> scope_f_table;
    // used to find type of function
    std::shared_ptr<node> start_of_scope;
    bool print_colour = true;
    void clear_node();
    node();
    ~node();
    std::shared_ptr<node> get_child(int i)
    {
        if (i >= this->children.size())
        {
            std::cerr << fmt::format("\nnode({} {}).get_child({})\n", WORD, UID, i);
            std::cerr << fmt::format("num children: {}\nChildren: ", this->children.size());
            for (auto c : this->children)
            {
                std::cout << fmt::format("{}({}), ", c->WORD, c->UID);
            }
            std::cerr << std::endl;
            std::cerr << "Attempted duplicates: ";
            for (auto d : this->attempted_duplicates)
            {
                std::cerr << fmt::format("[{}] {}, ", d.second, d.first);
            }
            std::cerr << std::endl;
            throw;
        }
        return this->children[i];
    };
    std::size_t num_children()
    {
        // std::cout << fmt::format("node({}).num_children()", UID);
        return this->children.size();
    };
    std::vector<std::shared_ptr<node>> &get_children()
    {
        // std::cout << fmt::format("node({}).get_children()", UID);
        return this->children;
    };
    std::size_t children_size()
    {
        // std::cout << fmt::format("node({}).children_size()", UID);
        return this->children.size();
    };
    bool add_child(std::shared_ptr<node> c, int index)
    {
        // std::cout << fmt::format("node({}).get_child(UID{})", UID, c->UID);
        if (child_ids.find(c->UID) != child_ids.end() || children.size() != index)
        {
            attempted_duplicates.push_back(std::pair<std::string, int>(fmt::format("{} {} {}", c->CLASS, c->UID, c->WORD), children.size()));
            std::cerr << fmt::format("\nDuplicate child (UID {}, WORD {}, CLASS {}) added to node (UID {}, WORD {}, CLASS {})\n", c->UID, c->WORD, c->CLASS, this->UID, this->WORD, this->CLASS);
            throw;
        }
        children.push_back(c);
        child_ids.emplace(c->UID);
        c->parent = shared_from_this();
        return true;
    };
    bool uid_exists(int UID)
    {
        return child_ids.find(UID) != child_ids.end();
    }
    bool subtree_generated = false;
    std::string printftable(std::shared_ptr<node> n);
    std::string printvtable(std::shared_ptr<node> n);
    std::string print_code(int depth, bool colour = false);
    // std::string printftable(std::shared_ptr<node> n)
    // {
    //     std::string ftable = "";
    //     for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
    //     {
    //         ftable = fmt::format("{}{}() ==> {}, ", ftable, it->first, it->second[0]);
    //     }
    //     return ftable;
    // }
    // std::string printvtable(std::shared_ptr<node> n)
    // {
    //     std::string vtable = "";
    //     for (auto it = n->v_table.begin(); it != n->v_table.end(); ++it)
    //     {
    //         vtable = fmt::format("{}{}: {}, ", vtable, it->first, it->second);
    //     }
    //     return vtable;
    // }

    // Drill down through successive chains of FUNCTIONS == > FUNCTIONS productions
    // static void copy_ftable(sym_table_type *f, std::shared_ptr<node> t);
    static void copy_ftable(std::shared_ptr<sym_table_type> f, std::shared_ptr<node> t);
    static void copy_vtable(sym_table_type *f, std::shared_ptr<node> t);
    static void copy_ftable(std::shared_ptr<node> f, std::shared_ptr<node> t);
    static void copy_vtable(std::shared_ptr<node> f, std::shared_ptr<node> t);
    static void copy_ftable(std::shared_ptr<sym_table_type> f, std::shared_ptr<sym_table_type> t);
    std::shared_ptr<node> parent;

private:
    std::vector<std::shared_ptr<node>> children; // Child nodes
    std::set<int> child_ids;
    std::vector<std::pair<std::string, int>> attempted_duplicates;
};

#endif