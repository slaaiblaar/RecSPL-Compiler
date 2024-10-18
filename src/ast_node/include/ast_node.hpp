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
class node : public std::enable_shared_from_this<node>
{
public:
    std::string NAME;  // Leaf or Internal node
    int UID;           // Unique identifier
    std::string CLASS; // Class of the node (nonterminal or terminal type)
    std::string WORD;  // Word representing the production
    std::string printnode(int depth, std::string called_from);
    // {v_name: type}
    std::unordered_map<std::string, std::string> v_table;
    // {f_name: [returntype, arg1, arg1, arg3]}
    std::unordered_map<std::string, std::string[4]> f_table;
    bool pre_processed = false;
    bool was_printed = false;
    bool is_in_scope = true;
    long node_id;
    static long node_id_counter;
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
    std::vector<std::shared_ptr<node>> get_children()
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
        return true;
    };
    bool uid_exists(int UID)
    {
        return child_ids.find(UID) != child_ids.end();
    }
    bool subtree_generated = false;
    std::string printftable(std::shared_ptr<node> n);
    std::string printvtable(std::shared_ptr<node> n);
    std::string print_code(int depth, std::ofstream &code_file);

private:
    std::vector<std::shared_ptr<node>> children; // Child nodes
    std::set<int> child_ids;
    std::vector<std::pair<std::string, int>> attempted_duplicates;
};

#endif