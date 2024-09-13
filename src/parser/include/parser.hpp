#ifndef parser_h
#define parser_h
#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
class Parser
{
public:
    Parser();
    void get_nullable();
    // void generate_first_sets();
    // void generate_follow_sets();
    // void read_token_stream(std::string input);

private:
    pugi::xml_document cfg_doc;
    // pugi::xml_parse_result token_stream;
    std::unordered_map<std::string, bool> nullable;
};
#endif
