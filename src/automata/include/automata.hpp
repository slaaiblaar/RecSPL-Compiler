#ifndef automata_h
#define automata_h
#include <vector>
#include <memory>        // shared_ptr
#include <unordered_map> // hashmap
#include <string>

class Token
{
public:
    Token(std::string type, std::string value);
    int token_id;
    std::string token_class;
    std::string token_word;
    static int tokenCounter;
};
// class Automata;
class State
{
    friend class Automaton;

public:
    State();
    int id;
    bool is_final = false;
    std::string token_class;
    void add_transition(std::shared_ptr<State> to, char soombol);
    bool can_transition(char soombol);

private:
    static int id_counter;
    // maps char to vector of states for NFA
    std::unordered_map<char, std::vector<std::shared_ptr<State>>> transitions;
};

enum AutomatonClass
{
    NFA,
    DFA
};

class Automaton
{
public:
    Automaton(AutomatonClass automaton_class);
    std::shared_ptr<State> string_to_automaton(std::string input, std::string token_class);
    bool pattern_to_automaton(std::string input, std::string token_class);
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
