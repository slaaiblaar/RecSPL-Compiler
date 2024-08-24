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
    // std::cout << "Destroying state " << id << std::endl;
}

void State::free()
{
    if (freeing)
    {
        return;
    }
    freeing = true;
    // std::cout << "Freeing state " << id << std::endl;
    for (auto transition : transitions)
    {
        for (auto state : transition.second)
        {
            state->free();
            state = nullptr;
        }
    }
    transitions.clear();
}
void State::add_transition(std::shared_ptr<State> to, char soombol)
{
    // std::cout << "+ " << id << " -(" << soombol << ")-> ";
    transitions[soombol].push_back(to);
    // std::cout << transitions[soombol][transitions.size() - 1]->id << std::endl;
}

bool State::can_transition(char soombol)
{
    return transitions.find(soombol) != transitions.end();
}