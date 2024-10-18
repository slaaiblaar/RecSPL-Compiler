#ifndef parser_h
#define parser_h
#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
#include <set>
#include "ast_node.hpp"

enum Operation
{
    SHIFT = 'S',
    GO = 'G',
    REDUCE = 'R',
    ACCEPT = 'A'
};
class Parser
{
public:
    Parser();
    void get_nullable();
    void generate_first_sets();
    void generate_follow_sets();
    std::shared_ptr<node> parse(std::string dest_name = "ast.xml");
    // void read_token_stream(std::string input);

private:
    pugi::xml_document cfg_doc;
    std::unordered_map<std::string, std::set<std::string>> first;  // FIRST sets
    std::unordered_map<std::string, std::set<std::string>> follow; // FOLLOW sets

    // pugi::xml_parse_result token_stream;
    std::unordered_map<std::string, bool> nullable;

    bool is_terminal(const std::string &symbol); // helper function
};
#endif
