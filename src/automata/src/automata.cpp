#include "automata.hpp"
#include <iostream>
#include <stack>
#include <algorithm>
#include <fstream>
#include <cctype>
// #include <pair>
Automaton::Automaton()
{
    nfa_start_state = std::make_shared<State>(AutomatonClass::NFA);
    nfa_start_state->e_closure[nfa_start_state->id] = nfa_start_state;
    add_state(nfa_start_state);
}
Automaton::~Automaton()
{
    if (nfa_start_state.use_count() > 0)
    {
        nfa_start_state->free();
    }
    if (dfa_start_state.use_count() > 0)
    {
        dfa_start_state->free();
    }
    nfa_states.clear();
    nfa_final_states.clear();
    dfa_states.clear();
    dfa_final_states.clear();
    // std::cout << "Destroying Automaton with dfa start state:\n ";
    // std::cout << dfa_start_state->id << "\n";
}

// simply strings a bunch of states together
std::shared_ptr<State> Automaton::append_keyword(std::string input, std::string token_class)
{

    int index = 0;
    std::shared_ptr<State> nfa_start = std::make_shared<State>(AutomatonClass::NFA);
    nfa_start->e_closure[nfa_start->id] = nfa_start;
    add_state(nfa_start);
    nfa_start_state->add_transition(nfa_start, '\0');
    std::shared_ptr<State> prev_state = nullptr;
    std::shared_ptr<State> current_state = nfa_start;
    while (index < input.length())
    {
        prev_state = current_state;
        current_state = std::make_shared<State>(AutomatonClass::NFA);
        current_state->e_closure[current_state->id] = current_state;
        add_state(current_state);
        add_transition(prev_state, current_state, input[index]);
        ++index;
    }
    current_state->is_final = true;
    nfa_final_states.push_back(current_state);
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

    return input;
}

// Kinda gross but it works
// TODO: Improve to handle more cases like [1-3], [A-G], etc.
std::string remove_bracket_shorthand(std::string input, int start)
{
    int pos = input.find("[A-Z]", start);
    if (pos != std::string::npos)
    {
        input.replace(pos, 5, "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)");
    }

    pos = input.find("[a-z]", start);
    while (pos != std::string::npos)
    {
        input.replace(pos, 5, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)");
        pos = input.find("[a-z]", start);
    }

    pos = input.find("[0-9]", start);
    while (pos != std::string::npos)
    {
        input.replace(pos, 5, "(0|1|2|3|4|5|6|7|8|9)");
        pos = input.find("[0-9]", start);
    }

    pos = input.find("[1-9]", start);
    while (pos != std::string::npos)
    {
        input.replace(pos, 5, "(1|2|3|4|5|6|7|8|9)");
        pos = input.find("[1-9]", start);
    }

    pos = input.find("[a-z0-9]", start);
    while (pos != std::string::npos)
    {
        input.replace(pos, 8, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9)");
        pos = input.find("[a-z0-9]", start);
    }

    return input;
}

bool Automaton::append_pattern(std::string input, std::string token_class)
{
    // Reduce number of possible regexp rules/operators
    input = remove_bracket_shorthand(input, 0);
    // Simplify pattern
    input = remove_parentheses_pairs(input);
    // Convert to tree structure
    std::shared_ptr<RegexpTree> tree = std::make_shared<RegexpTree>(input, 0);
    std::shared_ptr<State> nfa_start = std::make_shared<State>(AutomatonClass::NFA);
    nfa_start->e_closure[nfa_start->id] = nfa_start;
    add_state(nfa_start);
    std::shared_ptr<State> nfa_end = std::make_shared<State>(AutomatonClass::NFA);
    nfa_end->e_closure[nfa_end->id] = nfa_end;
    add_state(nfa_end);
    nfa_end->is_final = true;
    nfa_final_states.push_back(nfa_end);
    nfa_end->token_class = token_class;
    nfa_start_state->add_transition(nfa_start, '\0');
    tree_to_nfa(tree->root, nfa_start, nfa_end);
    return false;
}
// MARK: tree_to_nfa()
std::shared_ptr<State> Automaton::tree_to_nfa(std::shared_ptr<RegexpNode> node, std::shared_ptr<State> sub_nfa_start, std::shared_ptr<State> sub_nfa_end)
{
    std::shared_ptr<State> curr_state;
    if (node->value == '.' && node->precedence == 2)
    {
        curr_state = sub_nfa_start;
        std::shared_ptr<State> next_state;
        for (auto child : node->children)
        {
            next_state = std::make_shared<State>(AutomatonClass::NFA);
            next_state->e_closure[next_state->id] = next_state;
            add_state(next_state);
            curr_state = tree_to_nfa(child, curr_state, next_state);
            curr_state = next_state;
        }
        curr_state->add_transition(sub_nfa_end, '\0');
        return sub_nfa_end;
    }
    else if (node->value == '|')
    {
        for (auto child : node->children)
        {
            curr_state = std::make_shared<State>(AutomatonClass::NFA);
            curr_state->e_closure[curr_state->id] = curr_state;
            add_state(curr_state);

            sub_nfa_start->add_transition(curr_state, '\0');
            tree_to_nfa(child, curr_state, sub_nfa_end);
        }
        return sub_nfa_end;
    }
    else if (node->value == '*')
    {
        if (node->children.size() != 1)
        {
            std::cout << "Invalid * operator" << std::endl;
            throw "Invalid * operator";
        }
        curr_state = std::make_shared<State>(AutomatonClass::NFA);
        curr_state->e_closure[curr_state->id] = curr_state;
        add_state(curr_state);
        sub_nfa_start->add_transition(curr_state, '\0');
        sub_nfa_start->add_transition(sub_nfa_end, '\0');
        tree_to_nfa(node->children[0], curr_state, sub_nfa_start);
        return sub_nfa_end;
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
        curr_state = sub_nfa_start;
        curr_state->add_transition(sub_nfa_end, node->value);
        return sub_nfa_end;
    }
}
/*
    Helpers for NFA to DFA conversion
    MARK: construct_subsets()
*/
void Automaton::construct_subsets()
{
    std::cout << "Constructing Subsets\n";
    std::shared_ptr<State> current_state;
    /*
        2D hashmap is massively overkill, but it's the simplest way to prevent
        duplicate transitions. I can simply write `transitions[symbol][id] = ptr
        without worrying if there is already a transition for that state on that
        symbol
    */
    std::unordered_map<std::string, std::unordered_map<int, std::shared_ptr<State>>> transitions;
    while (!subset_construction_queue.empty())
    {
        current_state = subset_construction_queue.front();
        std::cout << current_state->id << " ";
        std::cout << "  Curr state: \n      " << current_state->id;
        subset_construction_queue.pop();
        transitions.clear();
        // for each nfa state in current state's epsilon closure set
        std::cout << "  NFA Equiv transitions: \n";
        for (auto nfa_equiv_state : current_state->nfa_equiv_states)
        {
            if (nfa_equiv_state.second->is_final)
            {
                std::cout << "    \033[33m" << nfa_equiv_state.second->id << "\033[0m";
            }
            else
                std::cout << "    " << nfa_equiv_state.second->id;
            std::cout << "    " << nfa_equiv_state.second->id;
            // for each transition from the nfa state
            for (auto e_state_transition : nfa_equiv_state.second->transitions)
            {
                std::string symbol = e_state_transition.first;
                if (symbol.length() == 0 || symbol[0] == '\0')
                {
                    continue;
                }
                std::vector<std::shared_ptr<State>> destination_states = e_state_transition.second;
                std::cout << " '\033[33m" << symbol << "\033[0m':";
                // for each destination on the symbol
                for (auto destination : destination_states)
                {
                    std::cout << " " << destination->id;
                    if (destination->is_final)
                    {
                        std::cout << "  \033[33m" << destination->id << "\033[0m";
                    }
                    else
                        std::cout << "  " << destination->id;
                    // for each state in the epsilon closure of the destination
                    for (auto dest_e_state : destination->e_closure)
                    {
                        transitions[symbol][dest_e_state.first] = dest_e_state.second;
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
        if (transitions.size() > 0)
        {
            std::cout << "\n    Combining transitions:\n      ";
        }
        // for each transition instantiated above
        for (auto transition : transitions)
        {
            // check if the set of nfa states already exists as a dfa state
            std::shared_ptr<State> destination = find_dfa_state(transition.second);
            // if not create one
            std::cout << " " << transition.first << " ";
            if (destination == nullptr)
            {
                destination = std::make_shared<State>(AutomatonClass::DFA);
                dfa_states.push_back(destination);
                // add set of nfa states to dfa state
                for (auto nfa_state : transition.second)
                {
                    destination->nfa_equiv_states[nfa_state.first] = nfa_state.second;
                    // make final if it contains a final nfa state
                    if (nfa_state.second->is_final)
                    {
                        destination->is_final = true;
                        if (destination->token_class != "")
                        {
                            std::cout << "\nOverlapping final states\n";
                            throw "Overlapping final states";
                        }
                        destination->token_class = nfa_state.second->token_class;
                        destination->lhs_name = nfa_state.second->lhs_name;
                        destination->rhs_nodes_list = nfa_state.second->rhs_nodes_list;
                        destination->prod_num = nfa_state.second->prod_num;
                    }
                }
                // add to construction queue
                subset_construction_queue.emplace(destination);
                if (destination->is_final)
                {
                    dfa_final_states.push_back(destination);
                }
                if (destination->is_final)
                {
                    std::cout << "(new \033[33m" << destination->id << "\033[0m)";
                }
                else
                    std::cout << "(new " << destination->id << ")";
            }
            // add transition to current state
            current_state->transitions[transition.first].push_back(destination);
        }
        std::cout << "\n";
        // std::cout << "FOLLOW Sets:\n";
        // // for (const auto &pair : this->follow)
        // {
        //     std::cout << pair.first << ": { ";
        //     for (const std::string &s : pair.second)
        //     {
        //         std::cout << "\"\033[33m" << s << "\033[0m\" ";
        //     }
        //     std::cout << "}\n";
        // }
    }
    // for (std::shared_ptr<State> s : dfa_states)
    // {
    //     if (s->is_final && s->nfa_equiv_states.size() > 1)
    //     {
    //         std::cout << s->id << ": \n";
    //         for (auto nfas : s->nfa_equiv_states)
    //         {
    //             if (nfas.second->is_final)
    //             {
    //                 std::cout << "    \033[33m" << nfas.first << "\033[0m ";
    //                 std::cout << "[" << nfas.second->prod_num << "](\033[33m" << nfas.second->lhs_name << "\033[0m ==> ";
    //                 for (auto n : nfas.second->rhs_nodes_list)
    //                 {
    //                     std::cout << "\033[33m" << n << "\033[0m ";
    //                 }
    //                 std::cout << ")\n";
    //             }
    //             else
    //                 std::cout << "    " << nfas.first;
    //         }
    //         std::cout << std::endl;
    //     }
    // }
}
void Automaton::add_state(std::shared_ptr<State> state)
{
    // std::cout << "Adding state: " << state->id << std::endl;
    nfa_states.push_back(state);
}

void Automaton::nfa_to_dfa()
{
    // reset IDs
    int idcounter = 0;
    for (auto s : this->nfa_states)
    {
        s->id = idcounter++;
    }
    // std::cout << "Epsilon transitions:\n";
    // for (auto current_state : nfa_states)
    // {
    //     std::cout << "S" << current_state->id << ":\n";
    //     std::cout << "\tstd::string({'\\0'}: ";
    //     for (std::shared_ptr<State> next_hop : current_state->transitions[std::string({'\0'})])
    //     {
    //         std::cout << next_hop->id << ", ";
    //     }
    //     std::cout << "\n\t\"\\0\": ";
    //     for (std::shared_ptr<State> next_hop : current_state->transitions["\0"])
    //     {
    //         std::cout << next_hop->id << ", ";
    //     }
    //     std::cout << std::endl;
    // }
    // calculate epsilon closure of nfa states
    bool e_closure_expanded = true;
    while (e_closure_expanded)
    {
        e_closure_expanded = false;
        for (auto current_state : nfa_states)
        {
            // std::cout << current_state->id
            // for each state in the current state's epsilon closure set
            for (std::pair<int, std::shared_ptr<State>> e_state : current_state->e_closure)
            {
                // for each epsilon transition of the above state
                for (std::shared_ptr<State> next_hop : e_state.second->transitions[std::string({'\0'})])
                {
                    if (current_state->e_closure.find(next_hop->id) == current_state->e_closure.end())
                    {
                        current_state->e_closure[next_hop->id] = next_hop;
                        e_closure_expanded = true;
                    }
                }
                for (std::shared_ptr<State> next_hop : e_state.second->transitions[""])
                {
                    if (current_state->e_closure.find(next_hop->id) == current_state->e_closure.end())
                    {
                        current_state->e_closure[next_hop->id] = next_hop;
                        e_closure_expanded = true;
                    }
                }
            }
        }
    }
    // std::cout << "E-closures\n";
    // for (auto state : nfa_states)
    // {
    //     std::cout << state->id << ": \n";
    //     for (auto s : state->e_closure)
    //     {
    //         if (s.second->token_class.length() > 0)
    //         {
    //             std::cout << "\t" << s.second->id << "(" << s.second->token_class << ") epsilons: \n";
    //             for (auto t : s.second->transitions)
    //             {
    //                 if (t.first.length() == 0)
    //                 {
    //                     std::cout << " \t\t\"\":";
    //                     for (auto dest : t.second)
    //                     {
    //                         std::cout << " " << dest->id;
    //                     }
    //                 }
    //                 else if (t.first == "\0" || (t.first.length() == 1 && (t.first[0] == '\0' || t.first[0] == 0)))
    //                 {
    //                     std::cout << " \t\t{";
    //                     for (auto c : t.first)
    //                     {
    //                         std::cout << "'" << ((int)c) << "', ";
    //                     }
    //                     std::cout << "}:";
    //                     for (auto dest : t.second)
    //                     {
    //                         std::cout << " " << dest->id;
    //                     }
    //                 }
    //                 std::cout << "\n";
    //             }
    //         }
    //         else
    //         {
    //             std::cout << s.first << ", ";
    //         }
    //     }
    //     std::cout << std::endl;
    // }
    // for (auto state : dfa_states)
    // {
    //     state->free();
    // }
    // clear possible old nfa
    dfa_states.clear();
    dfa_final_states.clear();
    // new start state with e closure of nfa start state
    dfa_start_state = std::make_shared<State>(AutomatonClass::DFA);
    dfa_states.push_back(dfa_start_state);
    for (auto e_state : nfa_start_state->e_closure)
    {
        dfa_start_state->nfa_equiv_states[e_state.first] = e_state.second;
        if (e_state.second->is_final)
        {
            if (dfa_start_state->is_final)
            {
                std::cout << "Collision on final state: \n";
                std::cout << "(old_prod|new_prod) : (" << dfa_start_state->prod_num;
                std::cout << "|" << e_state.second->prod_num << ")\n";
            }
            dfa_start_state->is_final = true;
            dfa_start_state->prod_num = e_state.second->prod_num;
            dfa_start_state->lhs_name = e_state.second->lhs_name;
            dfa_start_state->rhs_nodes_list = e_state.second->rhs_nodes_list;
            dfa_final_states.push_back(dfa_start_state);
        }
    }
    subset_construction_queue.push(dfa_start_state);
    // std::cout << "DFA Start State: " << dfa_start_state->id << " [" << dfa_start_state << "]\n";
    // subset_construction_queue.emplace(dfa_start_state);
    construct_subsets();
    // std::cout << "Subset construction Complete\n";
    this->current_state = dfa_start_state;
    this->accept_pos = -1;
    this->read_pos = 0;
    this->read_start = 0;
    this->accept_state = nullptr;
    idcounter = 0;
    // std::cout << "Resetting state Ids\n";

    for (auto s : this->dfa_states)
    {
        s->id = idcounter++;
    }
}
std::shared_ptr<State> Automaton::find_dfa_state(std::unordered_map<int, std::shared_ptr<State>> nfa_states)
{
    bool found = false;
    for (std::shared_ptr<State> state : dfa_states)
    {
        found = true;
        if (state->nfa_equiv_states.size() != nfa_states.size())
        {
            found = false;
            continue;
        }
        for (auto nfa_state : nfa_states)
        {
            if (state->nfa_equiv_states.find(nfa_state.first) == state->nfa_equiv_states.end())
            {
                found = false;
                break;
            }
        }
        if (found == true)
        {
            return state;
        }
    }
    return nullptr;
}

void Automaton::add_transition(std::shared_ptr<State> from, std::shared_ptr<State> to, char soombol)
{
    from->add_transition(to, soombol);
}

void Automaton::add_transition(int from, int to, char soombol)
{
    int from_id = -1, to_id = -1;
    for (int i = 0; i < nfa_states.size(); i++)
    {
        if (nfa_states[i]->id == from)
        {
            from_id = i;
        }
        if (nfa_states[i]->id == to)
        {
            to_id = i;
        }
    }
    if (from_id == -1 || to_id == -1)
    {
        return;
    }
    add_transition(nfa_states[from_id], nfa_states[to_id], soombol);
}

void Automaton::set_start_state(std::shared_ptr<State> state)
{
    nfa_start_state = state;
}

void Automaton::set_start_state(int id)
{
    for (int i = 0; i < nfa_states.size(); i++)
    {
        if (nfa_states[i]->id == id)
        {
            nfa_start_state = nfa_states[i];
            return;
        }
    }
}

void Automaton::set_final_state(std::shared_ptr<State> state)
{
    state->is_final = true;
    nfa_final_states.push_back(state);
}

void Automaton::set_final_state(int id)
{
    for (int i = 0; i < nfa_states.size(); i++)
    {
        if (nfa_states[i]->id == id)
        {
            nfa_states[i]->is_final = true;
            nfa_final_states.push_back(nfa_states[i]);
            return;
        }
    }
}

void Automaton::set_input(std::string input)
{
    line_num = 1;
    col_num = 0;
    this->input = input;
    current_state = dfa_start_state;
    read_start = 0;
    while (read_start < input.length() && isspace(input[read_start]))
    {
        // std::cout << "whitespace: {" << (long)input[read_start] << "}\n";
        if (input[read_start] == '\n')
        {
            ++line_num;
            col_num = 0;
        }
        else if (input[read_start] == 9)
        {
            col_num += 8;
        }
        else
        {
            ++col_num;
        }
        ++read_start;
    }
    read_pos = read_start;
    accept_pos = -1;
}

void Automaton::reset_dfa()
{
    this->current_state = dfa_start_state;
    col_num += accept_pos - read_start + 1;
    this->read_start = accept_pos + 1;
    this->accept_pos = -1;
    this->accept_state = nullptr;
    // progress read pos to next significant character
    while (read_start < input.length() && isspace(input[read_start]))
    {
        // std::cout << "whitespace: {" << (long)input[read_start] << "}\n";
        if (input[read_start] == 10)
        {
            ++line_num;
            col_num = 1;
        }
        else if (input[read_start] == 9)
        {
            col_num += 8;
        }
        else
        {
            ++col_num;
        }
        ++read_start;
    }
    read_pos = read_start;
}
// make a run from current start pos until an accept state is reached or invalid transition
// if accept state reached then update accept pos and return true
// if invalid transition then return false
bool Automaton::run(bool debug)
{
    if (debug)
    {
        std::cout << "Start state: " << dfa_start_state->id << "\n";
        std::cout << "Curr state:  " << current_state->id << "\n";
    }
    // input bound check
    if (accept_pos == read_pos)
    {
        ++read_pos;
    }
    if (debug)
    {

        std::cout << "RUNNING LEXER:\n";
        std::cout << "start pos: " << read_start << "\n";
    }
    while (read_pos < input.size())
    {
        if (debug)
        {
            std::cout << "Curr pos: " << read_pos << "\n";
        }
        char soombol = input[read_pos];
        if (debug)
        {
            for (int s = read_start; s <= read_pos; ++s)
                std::cout << input[s];
            std::cout << soombol << std::endl;
            std::cout << ": \tCAN TRANSITION FROM " << current_state << "?\n";
        }
        if (!current_state->can_transition(soombol, (this->input == "return")))
        {
            if (debug)
            {
                std::cout << "CAN'T TRANSITION\n";
                std::cout << "[Failed]\n";
            }
            return false;
        }
        if (debug)
        {
            std::cout << "NOT FAILED\n";
        }
        current_state = current_state->transitions[std::string({soombol})][0];
        if (current_state->is_final)
        {
            accept_pos = read_pos;
            if (debug)
            {
                std::cout << "[Accepted]\n";
            }
            return true;
        }
        if (debug)
        {
            std::cout << "[Failed]\n";
        }
        ++read_pos;
    }

    return false;
}

Token Automaton::get_token()
{
    std::string word = input.substr(read_start, accept_pos - read_start + 1);
    Token lex_token(word, current_state->token_class);
    lex_token.col = col_num;
    lex_token.row = line_num;
    return lex_token;
}

void Automaton::cfg_to_nfa(pugi::xml_node productions)
{
    this->was_cfg = true;
    // reset automaton
    if (nfa_start_state.use_count() > 0)
    {
        nfa_start_state->free();
    }
    if (dfa_start_state.use_count() > 0)
    {
        dfa_start_state->free();
    }
    nfa_states.clear();
    nfa_final_states.clear();
    dfa_states.clear();
    dfa_final_states.clear();

    nfa_start_state = std::make_shared<State>(AutomatonClass::NFA);
    nfa_start_state->is_final = false;
    nfa_start_state->e_closure[nfa_start_state->id] = nfa_start_state;
    add_state(nfa_start_state);
    // std::cout << "CFG NFA Start: " << nfa_start_state->id << std::endl;
    std::unordered_map<std::string, std::shared_ptr<State>> productions_map;
    pugi::xml_node lhs = productions.first_child();
    do
    {
        std::shared_ptr<State> curr_state = std::make_shared<State>(AutomatonClass::NFA);
        curr_state->e_closure[curr_state->id] = curr_state;
        // token_class is first 4 letters of name
        std::string tokenclass = lhs.name();
        curr_state->token_class = tokenclass.substr(0, 4);
        curr_state->is_final = false;
        productions_map[lhs.name()] = curr_state;
        // std::cout << "LHS: " << lhs.name() << std::endl;
        this->nfa_start_state->add_transition(curr_state, '\0');
        this->nfa_states.push_back(curr_state);
        // std::cout << "New NFA Node: " << curr_state->id << std::endl;
    } while ((lhs = lhs.next_sibling()) != pugi::xml_node());
    lhs = productions.first_child();
    do // while ((lhs = lhs.next_sibling()) != pugi::xml_node());
    {
        std::shared_ptr<State> curr_state;
        pugi::xpath_node_set rhs_nodes = lhs.select_nodes("production");
        std::shared_ptr<State> prev_state;
        std::string lhs_name = lhs.name();
        // lhs_name = lhs_name;
        for (pugi::xpath_node_set::const_iterator rhs_it = rhs_nodes.begin(); rhs_it != rhs_nodes.end(); ++rhs_it)
        {
            curr_state = productions_map[lhs.name()];
            std::vector<std::string> rhs_nodes_list = {};
            pugi::xml_node node = (*rhs_it).node().first_child();
            std::string symbol_str;
            do // while ((node = node.next_sibling()) != pugi::xml_node());
            {
                prev_state = curr_state;
                curr_state = std::make_shared<State>(AutomatonClass::NFA);
                curr_state->e_closure[curr_state->id] = curr_state;
                curr_state->is_final = false;
                symbol_str = node.name();
                std::string rhs_node_name = symbol_str;
                if (rhs_node_name == "KEYWORD")
                {
                    rhs_node_name = node.child("value").child_value();
                    // std::transform(rhs_node_name.begin(), rhs_node_name.end(), rhs_node_name.begin(), ::toupper);
                }
                rhs_nodes_list.push_back(rhs_node_name);
                if (node.attribute("terminal").as_bool())
                {
                    if (symbol_str == "KEYWORD")
                    {
                        symbol_str = node.child("value").child_value();
                    }
                    else if (symbol_str != "EPSILON")
                    {
                        symbol_str = node.first_child().child_value();

                        switch (symbol_str[0])
                        {
                        case '"':
                            symbol_str = "T_Token";
                            break;
                        case 'V':
                            symbol_str = "V_Token";
                            break;
                        case 'F':
                            symbol_str = "F_Token";
                            break;
                        default:
                            symbol_str = "N_Token";
                        }
                    }
                }
                else
                {
                    prev_state->add_transition(productions_map[node.name()], "\0");
                }
                prev_state->add_transition(curr_state, symbol_str);
                this->nfa_states.push_back(curr_state);
            } while ((node = node.next_sibling()) != pugi::xml_node());
            curr_state->is_final = true;
            curr_state->lhs_name = lhs.name();
            curr_state->rhs_nodes_list = rhs_nodes_list;
            curr_state->prod_num = prod_num;
            // special 0th production rule
            // if (symbol_str == "$")
            // {
            //     curr_state->prod_num = 0;
            //     --prod_num; // undo incoming increment
            // }
            ++prod_num;
            this->nfa_final_states.push_back(curr_state);
            // std::cout << curr_state->prod_num << " prod = " << curr_state->id << " state " << curr_state << std::endl;
            // std::cout << curr_state << " [" << curr_state->prod_num << "] : ";
            // std::cout << curr_state->lhs_name << " ==>";
            // for (std::string n : curr_state->rhs_nodes_list)
            // {
            //     std::cout << " " << n;
            // }
            // std::cout << std::endl;
            // std::cout << "[" << curr_state->prod_num << "] : ";
            // std::cout << curr_state->lhs_name << " ==>";
            // for (std::string n : curr_state->rhs_nodes_list)
            // {
            //     std::cout << " " << n;
            // }
            // std::cout << std::endl;
        }

    } while ((lhs = lhs.next_sibling()) != pugi::xml_node());
}

std::string Automaton::get_substring()
{
    if (accept_pos == -1)
    {
        return "";
    }
    return input.substr(read_start, accept_pos - read_start);
}
// Prints output to terminal that should go into render-automata.py
// MARK: print functions
void Automaton::print_nfa()
{
    std::ofstream python_generate_nfa("render-nfa.py");
    python_generate_nfa << "from automata.fa.nfa import NFA\nnfAutomaton = ";
    if (automaton_class == AutomatonClass::DFA)
    {
        python_generate_nfa << "DFA(" << std::endl;
    }
    else
    {
        python_generate_nfa << "NFA(" << std::endl;
    }
    std::unordered_map<std::string, std::vector<int>> allTransitions;
    std::vector<int> finalStates;
    python_generate_nfa << "    states={ ";
    for (auto state : nfa_states)
    {
        if (was_cfg && state->token_class.length() > 0)
        {
            python_generate_nfa << "\"" << state->token_class << "\", ";
        }
        else
        {
            python_generate_nfa << "\"S" << state->id << "\", ";
        }
    }
    python_generate_nfa << "},\n";
    python_generate_nfa << "    transitions={\n";
    for (auto state : nfa_states)
    {
        // if (state->id != 1621)
        // {
        //     std::cout << "\n\tState " << state->id << std::endl;
        //     std::cout << "\ttransitions: " << state->transitions.size() << std::endl;
        // }
        if (state->is_final)
        {
            // if (state->id != 1621)
            // {
            //     std::cout << "\tIs final" << std::endl;
            // }
            finalStates.push_back(state->id);
            continue;
        }
        // if (state->id != 1621)
        // {
        //     std::cout << "\tIs not final" << std::endl;
        // }
        // std::cout << "        \"S" << state->id << "\": { ";
        if (state->token_class.length() > 0)
        {
            python_generate_nfa << "\"" << state->token_class << "\": { ";
        }
        else
        {
            python_generate_nfa << "\"S" << state->id << "\": { ";
        }
        for (auto transition : state->transitions)
        {
            if (transition.first[0] != '\0' && transition.first != "")
            {
                allTransitions[transition.first].push_back(state->id);
            }
            // if-else if-else
            python_generate_nfa << "\"" << (transition.first[0] == '\0' ? "" : (transition.first[0] == '"' ? "\\\"" : transition.first)) << "\": { ";
            for (auto to_state : transition.second)
            {
                // python_generate_nfa << "\"S" << to_state->id << "\", ";
                if (was_cfg && to_state->token_class.length() > 0)
                {
                    python_generate_nfa << "\"" << to_state->token_class << "\", ";
                }
                else
                {
                    python_generate_nfa << "\"S" << to_state->id << "\", ";
                }
            }
            python_generate_nfa << "}, ";
        }
        python_generate_nfa << "},\n";
    }
    python_generate_nfa << "    },\n";
    python_generate_nfa << "    initial_state=\"S" << nfa_start_state->id << "\",\n";
    python_generate_nfa << "    final_states={ ";
    for (auto state : finalStates)
    {
        python_generate_nfa << "\"S" << state << "\", ";
    }
    python_generate_nfa << "},\n";
    python_generate_nfa << "    input_symbols={ ";
    for (auto transition : allTransitions)
    {
        python_generate_nfa << "\"" << (transition.first[0] == '"' ? "\\\"" : transition.first) << "\", ";
    }
    python_generate_nfa << "}\n)\nnfAutomaton.show_diagram(path=\"nfa_graph.png\")\n";
    python_generate_nfa.close();
}
std::string construct_final_name(std::shared_ptr<State> state)
{
    std::string prod = std::to_string(state->id);
    prod.append(" (");
    // prod.append(state->lhs_name);
    // for (int x = 0; x < state->rhs_nodes_list.size(); ++x)
    // {
    //     prod.append("-" + state->rhs_nodes_list[x]);
    // }
    prod.append(std::to_string(state->prod_num));
    prod.append(")");
    return prod;
}
void Automaton::print_dfa(std::string source_file, std::string fname)
{
    std::ofstream python_generate_dfa(source_file);
    python_generate_dfa << "from automata.fa.nfa import NFA\ndfAutomaton = ";
    python_generate_dfa << "NFA(" << std::endl;

    std::unordered_map<std::string, std::vector<int>> allTransitions;
    std::vector<std::string> finalStates;
    python_generate_dfa << "    states={ ";
    for (auto state : dfa_states)
    {
        if (state->is_final)
        {
            std::string prod = construct_final_name(state);
            python_generate_dfa << "\"" << prod << "\", ";
        }
        else
        {
            python_generate_dfa << "\"S" << state->id << "\", ";
        }
    }
    python_generate_dfa << "},\n";
    python_generate_dfa << "    transitions={\n";
    // get list of states
    for (auto state : dfa_states)
    {
        // lhs
        if (state->is_final)
        {
            std::string prod = construct_final_name(state);
            finalStates.push_back(prod);
            python_generate_dfa << "\"" << prod << "\": { ";
        }
        else
        {
            python_generate_dfa << "        \"S" << state->id << "\": { ";
        }
        // for each rhs of lhs
        for (auto transition : state->transitions)
        {
            // add transition symbol to map
            if (transition.first[0] != '\0' && transition.first != "")
            {
                allTransitions[transition.first].push_back(state->id);
            }
            // print symbol
            python_generate_dfa << "\"" << (transition.first[0] == '"' ? "\\\"" : transition.first) << "\": { ";
            // print destination state
            for (auto to_state : transition.second)
            {
                if (to_state->is_final)
                {
                    std::string prod = construct_final_name(to_state);
                    python_generate_dfa << "\"" << prod << "\", ";
                }
                else
                {
                    python_generate_dfa << "\"S" << to_state->id << "\", ";
                }
            }
            python_generate_dfa << "}, ";
        }
        python_generate_dfa << "},\n";
    }
    python_generate_dfa << "    },\n";
    python_generate_dfa << "    initial_state=\"S" << dfa_start_state->id << "\",\n";
    python_generate_dfa << "    final_states={ ";
    for (auto state : finalStates)
    {
        python_generate_dfa << "\"" << state << "\", ";
    }
    python_generate_dfa << "},\n";
    python_generate_dfa << "    input_symbols={ ";
    for (auto transition : allTransitions)
    {
        python_generate_dfa << "\"" << (transition.first[0] == '"' ? "\\\"" : transition.first) << "\", ";
    }
    python_generate_dfa << "}\n)\ndfAutomaton.show_diagram(path=\"" << fname << "\")\n";
    python_generate_dfa.close();
}