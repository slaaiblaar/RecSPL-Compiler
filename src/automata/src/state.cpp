#include "automata.hpp"
#include <iostream>
#include <stack>
thread_local int State::id_counter = 0;
thread_local std::unordered_map<std::string, int> State::op_precedence = {
    {"*", 1},
    {"+", 1},
    {"?", 1},
    {"|", 2},
    {"(", 4},
    {")", 4},
};
thread_local std::unordered_map<std::string, int> State::op_arity = {
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
    // std::cout << "Added transition " << this->id << " -\"" << soombol << "\"-> " << to->id << std::endl;
}
void State::add_transition(std::shared_ptr<State> to, std::string soombol)
{
    transitions[soombol].push_back(to);
    // std::cout << "Added transition " << this->id << " -\"" << soombol << "\"-> " << to->id << std::endl;
}

bool State::can_transition(char soombol, bool debug)
{
    // std::cout << "STATE " << id << "transitions: \n";
    // for (auto t : transitions)
    // {
    //     std::cout << t.first << ": " << t.second[0]->id << "\n";
    // }
    auto states = transitions.find(std::string({soombol}));
    if (states == transitions.end())
    {
        if (debug)
        {

            std::cout << "STATE " << id << " has no transition on symbol " << soombol << "\n";
        }
        return false;
    }
    if (states->second.size() == 0)
    {
        if (debug)
        {

            std::cout << "STATE " << id << " has no destination for symbol " << soombol << "\n";
        }
        return false;
    }
    if (states->second[0] == nullptr || states->second[0] == 0)
    {
        if (debug)
        {

            std::cout << "STATE " << id << " has a null destination on " << soombol << "\n";
        }
        return false;
    }
    if (debug)
    {

        std::cout << "STATE " << id << " has a transition " << soombol << " to " << states->second[0] << "\n";
    }
    return true;
}