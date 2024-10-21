#include "automata.hpp"
#include "pugixml.hpp"
#include <unordered_map> // hashmap
#include <set>
#include "scope_checker.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include "type_checker.hpp"
#include "code_generator.hpp"
#ifndef testing_h
#define testing_h
enum component
{
    LEXER,
    PARSER,
    SCOPE_CHECK,
    TYPE_CHECK,
    CODE_GEN
};

typedef struct
{
    int num_functions;
    int num_branches;
    int num_number_v;
    int num_text_v;
    int num_number_f;
    int num_void_f;
} counters;
class Tester
{
public:
    int generate_tree(std::shared_ptr<node> parent, pugi::xml_node productions, int depth, component test, counters counters = {0, 0, 0, 0, 0, 0});
    void run_tests(int thread_number);
    void test_lexer(int thread_number, std::vector<std::string> &results);
    void test_parser(int thread_number, std::vector<std::string> &results);
    void test_scope_checker(int thread_number, std::vector<std::string> &results);
    void test_type_checker(int thread_number, std::vector<std::string> &results);
    void test_code_generator();
    int num_terminals = 0;
    void populate_identifiers(std::shared_ptr<node> n, component test);
    void construct_ftables(std::shared_ptr<node> n, int depth, component test);
    std::shared_ptr<sym_table_type> preprocess_ftables(std::shared_ptr<node> n, int depth);
    std::string print_code(std::shared_ptr<node> n);
    std::pair<int, std::string> messed_up_word = std::pair<int, std::string>(-1, "NONE");
    std::vector<std::pair<std::string, std::shared_ptr<node>>> scope_errors;
    std::vector<std::pair<std::string, std::shared_ptr<node>>> discovered_scope_errors;
    std::string cfg_file = "CFG.xml";
    std::set<std::string> used_names;
};
#endif
