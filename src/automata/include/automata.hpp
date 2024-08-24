#ifndef automata_h
#define automata_h
#include <vector>
#include <memory>        // shared_ptr
#include <unordered_map> // hashmap
#include <string>
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
    static int uidCounter;
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
    Token(std::string type, std::string value);
    int token_id;
    std::string token_class;
    std::string token_word;
    static int tokenCounter;
};

enum AutomatonClass
{
    NFA,
    DFA
};
class State : public std::enable_shared_from_this<State>
{
    friend class Automaton;

public:
    State(AutomatonClass state_class);
    ~State();
    int id;
    std::vector<int> nfa_ids;
    // differentiate between NFA states with one ID and DFA states with sets IDs
    AutomatonClass state_class;
    bool is_final = false;
    bool freeing = false;
    std::string token_class;
    void add_transition(std::shared_ptr<State> to, char soombol);
    bool can_transition(char soombol);
    void free();

private:
    static int id_counter;
    static std::unordered_map<std::string, int> op_precedence;
    static std::unordered_map<std::string, int> op_arity;
    // maps char to vector of states for NFA
    std::unordered_map<char, std::vector<std::shared_ptr<State>>> transitions;
};

class Automaton : public std::enable_shared_from_this<Automaton>
{
public:
    Automaton();
    ~Automaton();
    std::shared_ptr<State> append_keyword(std::string input, std::string token_class);
    std::string get_next_token();
    bool append_pattern(std::string input, std::string token_class);
    std::shared_ptr<State> tree_to_nfa(std::shared_ptr<RegexpNode> node, std::shared_ptr<State> nfa_start, std::shared_ptr<State> nfa_end);
    void add_state(std::shared_ptr<State> state);
    void add_transition(std::shared_ptr<State> from, std::shared_ptr<State> to, char soombol);
    void add_transition(int from, int to, char soombol);
    void set_start_state(std::shared_ptr<State> state);
    void set_start_state(int id);
    void set_final_state(std::shared_ptr<State> state);
    void set_final_state(int id);
    void set_input(std::string input);
    void reset();
    bool run(std::string input);
    std::string get_substring();
    std::shared_ptr<RegexpTree> tree;
    void print_automaton();

private:
    AutomatonClass automaton_class;
    std::shared_ptr<State> start_state;
    std::shared_ptr<State> current_state;
    int read_start = 0;
    int read_pos = 0;
    int accept_pos = 0;
    std::vector<std::shared_ptr<State>> states;
    std::string input;
};

#endif
