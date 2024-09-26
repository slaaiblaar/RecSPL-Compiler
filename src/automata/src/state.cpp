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
State::State(AutomatonClass state_class)
    : state_class(state_class)
{
    id = id_counter++;
}
State::~State()
{
    // std::cout << "Destroying " << (this->state_class == AutomatonClass::NFA ? "NFA" : "DFA") << " State " << id << std::endl;
}

void State::free()
{
    // prevent double free
    if (freeing)
    {
        return;
    }
    // std::cout << "Freeing " << (this->state_class == AutomatonClass::NFA ? "NFA" : "DFA") << " State " << this->id << std::endl;
    freeing = true;
    // std::cout << "Freeing state " << id << std::endl;
    for (auto transition : transitions)
    {
        for (auto state : transition.second)
        {
            // prevent freeing NFA states when clearing DFA
            // and vice versa
            if (state->state_class == this->state_class)
            {
                state->free();
                state = nullptr;
            }
        }
    }
    transitions.clear();
    for (auto nfa_state : nfa_equiv_states)
    {
        nfa_state.second->free();
    }
    nfa_equiv_states.clear();
    for (auto e_state : e_closure)
    {
        e_state.second->free();
    }
    e_closure.clear();
}
void State::add_transition(std::shared_ptr<State> to, char soombol)
{
    transitions[std::string({soombol})].push_back(to);
}
void State::add_transition(std::shared_ptr<State> to, std::string soombol)
{
    transitions[soombol].push_back(to);
}

bool State::can_transition(char soombol)
{
    return transitions.find(std::string({soombol})) != transitions.end();
}