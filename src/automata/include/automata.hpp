#ifndef automata_h
#define automata_h
#include <vector>
#include <memory>        // shared_ptr
#include <unordered_map> // hashmap
#include <string>
#include <queue>
#include "pugixml.hpp"
#include <thread>
// Regexp Tree Classes
class RegexpNode : public std::enable_shared_from_this<RegexpNode>
{
public:
    RegexpNode(char value);
    ~RegexpNode();
    int uid;
    // contains both operators and character literals
    // operators: |, *, . (concatenation)   (, )
    // () operators not needed because they are implicit in the tree structure
    // The concatenation operator clashes with the decimal point character,
    // but this is not a problem because they have different precedences
    // literals have precedence 5
    // operators have precedence 1 (|), 2(.), 3(*)
    char value;
    int precedence;
    std::vector<std::shared_ptr<RegexpNode>> children;
    std::shared_ptr<RegexpNode> parent;
    void addChild(std::shared_ptr<RegexpNode> child);
    void printNode(int depth, int offset);
    void free();
    void flattenNode();
    thread_local static int uidCounter;
};
class RegexpTree
{
public:
    RegexpTree(std::string input, int subtreeDepth);
    ~RegexpTree();
    std::shared_ptr<RegexpNode> root;
    std::string input;
    int groupLength(int index, int offset = 0);
    void concatenateTree(std::shared_ptr<RegexpTree> subTree);
    void printTree();
};
// Automata Classes
class Token
{
public:
    Token(std::string word, std::string token_class);
    int token_id;
    std::string token_class;
    std::string token_word;
    thread_local static int token_counter;
    int row = -1;
    int col = -1;
};

enum AutomatonClass
{
    NFA = 0,
    DFA = 1
};
class State : public std::enable_shared_from_this<State>
{
    friend class Automaton;

public:
    State(AutomatonClass state_class);
    ~State();
    int id;
    std::unordered_map<int, std::shared_ptr<State>> nfa_equiv_states;
    // maps char to vector of states for NFA
    std::unordered_map<std::string, std::vector<std::shared_ptr<State>>> transitions;
    // differentiate between NFA states with one ID and DFA states with ID sets
    AutomatonClass state_class;
    bool is_final = false;
    bool freeing = false;
    std::string token_class;
    void add_transition(std::shared_ptr<State> to, char soombol);
    void add_transition(std::shared_ptr<State> to, std::string soombol);
    bool can_transition(char soombol, bool debug = false);
    void free();
    std::vector<std::string> rhs_nodes_list = {};
    std::string lhs_name = "";
    int prod_num = -1;
    bool dummy_bool = false;

private:
    thread_local static int id_counter;
    thread_local static std::unordered_map<std::string, int> op_precedence;
    thread_local static std::unordered_map<std::string, int> op_arity;
    std::unordered_map<int, std::shared_ptr<State>> e_closure;
};

class Automaton : public std::enable_shared_from_this<Automaton>
{
public:
    Automaton();
    ~Automaton();
    std::shared_ptr<State> append_keyword(std::string input, std::string token_class);
    bool append_pattern(std::string input, std::string token_class);
    std::shared_ptr<State> tree_to_nfa(std::shared_ptr<RegexpNode> node, std::shared_ptr<State> nfa_start, std::shared_ptr<State> nfa_end);
    void nfa_to_dfa();
    void add_state(std::shared_ptr<State> state);
    void add_transition(std::shared_ptr<State> from, std::shared_ptr<State> to, char soombol);
    void add_transition(int from, int to, char soombol);
    void set_start_state(std::shared_ptr<State> state);
    void set_start_state(int id);
    void set_final_state(std::shared_ptr<State> state);
    void set_final_state(int id);
    void set_input(std::string input);
    void reset_dfa();
    bool run(bool debug = false);
    std::shared_ptr<State> find_dfa_state(std::unordered_map<int, std::shared_ptr<State>> nfa_states);
    std::string get_substring();
    std::shared_ptr<RegexpTree> tree;
    void print_nfa();
    void print_dfa(std::string source_file = "render-dfa.py", std::string fname = "dfa_graph.png");
    void construct_subsets();
    Token get_token();
    void cfg_to_nfa(pugi::xml_node productions);
    int prod_num = 0;
    // private:
    AutomatonClass automaton_class;
    std::shared_ptr<State> nfa_start_state;
    std::vector<std::shared_ptr<State>> nfa_states;
    std::vector<std::shared_ptr<State>> nfa_final_states;
    std::shared_ptr<State> dfa_start_state;
    std::vector<std::shared_ptr<State>> dfa_states;
    std::vector<std::shared_ptr<State>> dfa_final_states;
    std::shared_ptr<State> current_state;
    int read_start = 0;
    int read_pos = 0;
    int accept_pos = 0;
    std::shared_ptr<State> accept_state = nullptr;
    std::string input;
    std::queue<std::shared_ptr<State>> subset_construction_queue;
    bool was_cfg = false;
    int line_num;
    int col_num;
};

#endif
