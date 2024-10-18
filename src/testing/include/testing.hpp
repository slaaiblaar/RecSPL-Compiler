#ifndef testing_h
#define testing_h
#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
#include <set>
#include "scope_checker.hpp"
#include "parser.hpp"
#include "lexer.hpp"
enum component
{
    LEXER,
    PARSER,
    SCOPE_CHECK,
    TYPE_CHECK,
    CODE_GEN
};
class Tester
{
public:
    int generate_tree(std::shared_ptr<node> parent, pugi::xml_node productions, int depth, component test);
    void run_tests(int thread_number);
    void test_lexer();
    void test_parser();
    void test_scope_checker();
    int num_terminals = 0;
    void populate_identifiers(std::shared_ptr<node> n, component test);
    void construct_ftables(std::shared_ptr<node> n, int depth, component test);
    std::pair<int, std::string> messed_up_word = std::pair<int, std::string>(-1, "NONE");
    std::string cfg_file = "CFG.xml";
};
#endif
