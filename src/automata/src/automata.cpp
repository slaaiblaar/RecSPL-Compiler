#include "automata.hpp"
#include <iostream>
#include <stack>
Automaton::Automaton()
{
    // std::cout << "Automaton()" << std::endl;
    start_state = std::make_shared<State>(AutomatonClass::NFA);
    add_state(start_state);
}
Automaton::~Automaton()
{
    // std::cout << "~Automaton()" << std::endl;
    if (start_state.use_count() > 0)
    {
        start_state->free();
    }
    // std::cout << "Automaton destroyed" << std::endl;
    std::cout << tree.use_count() << std::endl;
}

// simply strings a bunch of states together
std::shared_ptr<State> Automaton::append_keyword(std::string input, std::string token_class)
{
    // if NFA then add epsilon transition from start state to first state
    // if (automaton_class != AutomatonClass::NFA)
    // {
    //     return nullptr;
    // }

    int index = 0;
    std::shared_ptr<State> nfa_start = std::make_shared<State>(AutomatonClass::NFA);
    add_state(nfa_start);
    start_state->add_transition(nfa_start, '\0');
    std::shared_ptr<State> prev_state = nullptr;
    std::shared_ptr<State> current_state = nfa_start;
    while (index < input.length())
    {
        prev_state = current_state;
        current_state = std::make_shared<State>(AutomatonClass::NFA);
        add_state(current_state);
        add_transition(prev_state, current_state, input[index]);
        ++index;
    }
    current_state->is_final = true;
    current_state->token_class = token_class;

    return nfa_start;
}

/**
 * pattern helpers
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
                // remove closing and opening pair
                input.erase(pos, 1);
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

bool Automaton::append_pattern(std::string input, std::string token_class)
{
    // Reduce number of possible regexp rules/operators
    input = remove_bracket_shorthand(input, 0, input.length());
    // Simplify pattern
    input = remove_parentheses_pairs(input);
    // Convert to tree structure
    // RegexpTree tree(input, 0);
    // tree.printTree();
    std::shared_ptr<RegexpTree> tree = std::make_shared<RegexpTree>(input, 0);
    tree->printTree();
    std::cout << "Tree use count: " << tree.use_count() << std::endl;
    std::cout << "Creating NFA" << std::endl;
    std::shared_ptr<State> nfa_start = std::make_shared<State>(AutomatonClass::NFA);
    add_state(nfa_start);
    std::shared_ptr<State> nfa_end = std::make_shared<State>(AutomatonClass::NFA);
    add_state(nfa_end);
    nfa_end->is_final = true;
    nfa_end->token_class = token_class;
    std::cout << "Adding transition from start to nfa_start" << std::endl;
    start_state->add_transition(nfa_start, '\0');
    tree_to_nfa(tree->root, nfa_start, nfa_end);
    return false;
}

std::shared_ptr<State> Automaton::tree_to_nfa(std::shared_ptr<RegexpNode> node, std::shared_ptr<State> nfa_start, std::shared_ptr<State> nfa_end)
{
    std::shared_ptr<State> curr_state;
    if (node->value == '.' && node->precedence == 2)
    {
        curr_state = nfa_start;
        std::shared_ptr<State> next_state;
        for (auto child : node->children)
        {
            next_state = std::make_shared<State>(AutomatonClass::NFA);
            add_state(next_state);
            curr_state = tree_to_nfa(child, curr_state, next_state);
            curr_state = next_state;
        }
        // curr_state->add_transition(nfa_end, '\0');
        // return nfa_end;
        curr_state->add_transition(nfa_end, '\0');
        return nfa_end;
    }
    else if (node->value == '|')
    {
        std::cout << "S" << nfa_start->id << ": OR operator" << std::endl;
        // std::shared_ptr<State> end = std::make_shared<State>(AutomatonClass::NFA);
        for (auto child : node->children)
        {
            curr_state = std::make_shared<State>(AutomatonClass::NFA);
            add_state(curr_state);

            nfa_start->add_transition(curr_state, '\0');
            std::cout << "S" << nfa_start->id << " -(" << '\0' << ")-> S" << curr_state->id << std::endl;
            tree_to_nfa(child, curr_state, nfa_end);

            // child_end->add_transition(end, '\0');
        }
        // end->add_transition(nfa_end, '\0');
        return nfa_end;
    }
    else if (node->value == '*')
    {
        if (node->children.size() != 1)
        {
            std::cout << "Invalid * operator" << std::endl;
            throw "Invalid * operator";
        }
        curr_state = std::make_shared<State>(AutomatonClass::NFA);
        add_state(curr_state);
        nfa_start->add_transition(curr_state, '\0');
        nfa_start->add_transition(nfa_end, '\0');
        tree_to_nfa(node->children[0], curr_state, nfa_start);
        return nfa_end;
    }
    else
    {
        if (node->children.size() != 0 || node->precedence != 5)
        {
            std::cout << "Invalid node" << std::endl;
            std::cout << "[" << node->value << "]" << "(" << node->precedence << ")" << "-" << node->uid << std::endl;
            std::cout << node->children.size() << " children: ";
            for (auto child : node->children)
            {
                std::cout << "[" << child->value << "]" << "(" << child->precedence << ")" << "-" << child->uid << " ";
            }
            throw "Invalid operator";
        }
        curr_state = nfa_start;
        curr_state->add_transition(nfa_end, node->value);
        return nfa_end;
    }
}

void Automaton::add_state(std::shared_ptr<State> state)
{
    // std::cout << "Adding state: " << state->id << std::endl;
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
// Prints output to terminal that is should go into render-automata.py
void Automaton::print_automaton()
{
    std::cout << "NFA Rendering:\n```copy below\nfrom automata.fa.nfa import NFA\nautomaton = ";
    if (automaton_class == AutomatonClass::DFA)
    {
        std::cout << "DFA(" << std::endl;
    }
    else
    {
        std::cout << "NFA(" << std::endl;
    }
    std::unordered_map<char, std::vector<int>> allTransitions;
    std::vector<int> finalStates;
    std::cout << "    states={ ";
    for (auto state : states)
    {
        std::cout << "\"S" << state->id << "\", ";
    }
    std::cout << "},\n";
    std::cout << "    transitions={\n";
    for (auto state : states)
    {
        if (state->is_final)
        {
            finalStates.push_back(state->id);
            // std::cout << "\t Accept Class: " << state->token_class << std::endl;
            continue;
        }
        std::cout << "        \"S" << state->id << "\": { ";
        for (auto transition : state->transitions)
        {
            if (transition.first != '\0')
            {
                allTransitions[transition.first].push_back(state->id);
            }
            std::cout << "\"" << transition.first << "\": { ";
            for (auto to_state : transition.second)
            {
                std::cout << "\"S" << to_state->id << "\", ";
            }
            std::cout << "}, ";
        }
        std::cout << "},\n";
    }
    std::cout << "    },\n";
    std::cout << "    initial_state=\"S" << start_state->id << "\",\n";
    std::cout << "    final_states={ ";
    for (auto state : finalStates)
    {
        std::cout << "\"S" << state << "\", ";
    }
    std::cout << "},\n";
    std::cout << "    input_symbols={ ";
    for (auto transition : allTransitions)
    {
        std::cout << "\"" << transition.first << "\", ";
    }
    std::cout << "}\n)\nautomaton.show_diagram(path=\"graph.png\")\n```\n";
}