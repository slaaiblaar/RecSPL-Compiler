#include "automata.hpp"
#include <iostream>
#include <stack>
int State::id_counter = 0;
std::unordered_map<std::string, int> State::op_precedence = {
    {"*", 1},
    {"+", 1},
    {"?", 1},
    {"|", 2},
    {"(", 4},
    {")", 4},
};
std::unordered_map<std::string, int> State::op_arity = {
    {"*", 1},
    {"+", 1},
    {"?", 1},
    {"|", 2},
    {"(", 0},
    {")", 0},
};
State::State()
{
    id = id_counter++;
}

void State::add_transition(std::shared_ptr<State> to, char soombol)
{
    transitions[soombol].push_back(to);
}

bool State::can_transition(char soombol)
{
    return transitions.find(soombol) != transitions.end();
}

Automaton::Automaton(AutomatonClass automaton_class) : automaton_class(automaton_class)
{
    start_state = nullptr;
    current_state = nullptr;
    // if NFA create a start state for epsilon transitions
    if (automaton_class == AutomatonClass::NFA)
    {
        start_state = std::make_shared<State>();
        states.push_back(start_state);
    }
}

// simply strings a bunch of states together
std::shared_ptr<State> Automaton::string_to_automaton(std::string input, std::string token_class)
{
    // if NFA then add epsilon transition from start state to first state
    if (automaton_class != AutomatonClass::NFA)
    {
        return nullptr;
    }

    int index = 0;
    std::shared_ptr<State> nfa_start = std::make_shared<State>();
    std::shared_ptr<State> prev_state = nullptr;
    std::shared_ptr<State> current_state = nfa_start;
    states.push_back(nfa_start);
    while (index < input.length())
    {
        prev_state = current_state;
        current_state = std::make_shared<State>();
        states.push_back(current_state);
        add_transition(prev_state, current_state, input[index]);
        index++;
    }
    current_state->is_final = true;
    current_state->token_class = token_class;
    start_state->add_transition(nfa_start, '\0');

    return nfa_start;
}