#include "automata.hpp"
#include <iostream>
#include <stack>
/**
 * pattern helpers in order of relevant operator precedence
 */
std::string remove_parentheses_pairs(std::string input)
{
    std::stack<int> open_parentheses_positions;
    int start = 0;
    // find all open parentheses pairs
    int open_pos = input.find("((", start);
    while (open_pos != std::string::npos)
    {
        open_parentheses_positions.push(open_pos);
        start = open_pos + 1;
        open_pos = input.find("((", start);
    }
    if (open_parentheses_positions.empty())
    {
        return input;
    }
    while (!open_parentheses_positions.empty())
    {
        open_pos = open_parentheses_positions.top();
        open_parentheses_positions.pop();
        // check if matching closing parentheses pair exists. if not, then continue
        int close_pos = input.find("))", open_pos);
        if (close_pos != std::string::npos)
        {

            // check if these are matching parentheses pairs. if not, then continue
            int parentheses_count = 0;
            int pos = open_pos - 1;
            do
            {
                ++pos;
                if (input[pos] == '(')
                {
                    ++parentheses_count;
                }
                else if (input[pos] == ')')
                {
                    --parentheses_count;
                }
            } while (pos < input.length() && parentheses_count > 0);
            // malformed parentheses pairs
            if (pos == input.length())
            {
                break;
            }
            // input[pos] == ')' is assumed
            if (input[pos - 1] == ')')
            {
                // remove closing pair
                input.erase(pos, 1);
                // remove opening pair
                input.erase(open_pos, 1);
            }
        }
    }

    std::cout << "Simplified parentheses: " << input << std::endl;
    return input;
}

// Kinda gross but it works
// TODO: Improve to handle more cases like [1-3], [A-G], etc.
std::string remove_bracket_shorthand(std::string input, int start, int end)
{
    std::cout << "Removing bracket shorthands for input: " << input << std::endl;
    int pos = input.find("[A-Z]", start);
    std::cout << "Expanding [A-Z]: ";
    if (pos != std::string::npos)
    {
        input.replace(pos, 5, "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)");
    }
    std::cout << input << std::endl;

    pos = input.find("[a-z]", start);
    std::cout << "Expanding [a-z]: ";
    while (pos != std::string::npos)
    {
        input.replace(pos, 5, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)");
        pos = input.find("[a-z]", start);
    }
    std::cout << input << std::endl;

    pos = input.find("[0-9]", start);
    std::cout << "Expanding [0-9]: ";
    while (pos != std::string::npos)
    {
        input.replace(pos, 5, "(0|1|2|3|4|5|6|7|8|9)");
        pos = input.find("[0-9]", start);
    }
    std::cout << input << std::endl;

    pos = input.find("[1-9]", start);
    std::cout << "Expanding [1-9]: ";
    while (pos != std::string::npos)
    {
        input.replace(pos, 5, "(1|2|3|4|5|6|7|8|9)");
        pos = input.find("[1-9]", start);
    }
    std::cout << input << std::endl;

    pos = input.find("[a-z0-9]", start);
    std::cout << "Expanding [a-z0-9]: ";
    while (pos != std::string::npos)
    {
        input.replace(pos, 8, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9)");
        pos = input.find("[a-z0-9]", start);
    }
    std::cout << input << std::endl;

    return input;
}

bool Automaton::pattern_to_automaton(std::string input, std::string token_class)
{
    std::cout << "hey" << std::endl;
    if (automaton_class != AutomatonClass::NFA || input.length() == 0)
    {
        return false;
    }

    // Reduce number of possible regexp rules/operators
    input = remove_bracket_shorthand(input, 0, input.length());
    // Simplify pattern
    input = remove_parentheses_pairs(input);
    // Convert to tree structure
    RegexpTree tree(input, 0);
    tree.printTree();

    return false;
}

void Automaton::add_state(std::shared_ptr<State> state)
{
    states.push_back(state);
}

void Automaton::add_transition(std::shared_ptr<State> from, std::shared_ptr<State> to, char soombol)
{
    from->add_transition(to, soombol);
}

void Automaton::add_transition(int from, int to, char soombol)
{
    int from_id = -1, to_id = -1;
    for (int i = 0; i < states.size(); i++)
    {
        if (states[i]->id == from)
        {
            from_id = i;
        }
        if (states[i]->id == to)
        {
            to_id = i;
        }
    }
    if (from_id == -1 || to_id == -1)
    {
        return;
    }
    add_transition(states[from_id], states[to_id], soombol);
}

void Automaton::set_start_state(std::shared_ptr<State> state)
{
    start_state = state;
}

void Automaton::set_start_state(int id)
{
    for (int i = 0; i < states.size(); i++)
    {
        if (states[i]->id == id)
        {
            start_state = states[i];
            return;
        }
    }
}

void Automaton::set_final_state(std::shared_ptr<State> state)
{
    state->is_final = true;
}

void Automaton::set_final_state(int id)
{
    for (int i = 0; i < states.size(); i++)
    {
        if (states[i]->id == id)
        {
            states[i]->is_final = true;
            return;
        }
    }
}

void Automaton::set_input(std::string input)
{
    this->input = input;
    current_state = start_state;
    read_pos = 0;
    read_start = 0;
    accept_pos = -1;
}

void Automaton::reset()
{
    current_state = start_state;
    read_start = read_pos;
    accept_pos = -1;
}
// make a run from current start pos until an accept state is reached or invalid transition
// if accept state reached then update accept pos and return true
// if invalid transition then return false
bool Automaton::run(std::string input)
{
    // input bound check
    while (read_pos < input.size())
    {
        char soombol = input[read_pos];
        if (!current_state->can_transition(soombol))
        {
            return false;
        }
        current_state = current_state->transitions[soombol][0];
        read_pos++;
        if (current_state->is_final)
        {
            accept_pos = read_pos;
            return true;
        }
    }
    return false;
}

std::string Automaton::get_substring()
{
    if (accept_pos == -1)
    {
        return "";
    }
    return input.substr(read_start, accept_pos - read_start);
}
