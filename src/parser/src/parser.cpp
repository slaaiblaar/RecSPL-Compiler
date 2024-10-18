#include "parser.hpp"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>
#include <cstdlib>
#include <regex>
Parser::Parser()
{
    std::cout << "PARSER\n\n\n";
    pugi::xml_parse_result result = this->cfg_doc.load_file("./CFG.xml");
    std::cout << "FILE LOADED\n";
    // really shouldn't happen but w/e, TS habits
    if (!result)
    {
        std::cout << "CFG XML file does not exist\n";
        return;
    }
    this->get_nullable();
    std::cout << "Nullable\n";
    this->generate_first_sets();
    std::cout << "First\n";
    this->generate_follow_sets();
    std::cout << "Follow\n";
}

void Parser::get_nullable()
{
    std::cout << "Getting nullable values\n";
    pugi::xml_node productions = this->cfg_doc.child("CFG").child("PRODUCTIONRULES");
    pugi::xml_node non_terminal = productions.first_child();

    // initialise nullable values
    do
    {
        this->nullable[non_terminal.name()] = false;
    } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());

    bool changed = true;
    while (changed)
    {
        non_terminal = productions.first_child();
        changed = false;
        do
        {
            // std::cout << "Evaluating non terminal: " << non_terminal.name() << std::endl;
            // skip if already nullable
            if (this->nullable[non_terminal.name()])
            {
                continue;
            }
            pugi::xpath_node epsilon = non_terminal.select_node("production/EPSILON");
            // if terminal exists then set nullable
            if (epsilon != pugi::xml_node())
            {
                // std::cout << non_terminal.name() << " has epsilon rhs\n";
                this->nullable[non_terminal.name()] = true;
                changed = true;
                continue;
            }
            // iterate through right hand sides to look for non nullable symbols
            // get all <production> children of non_nullable
            // std::cout << "Manual search\n";
            pugi::xpath_node_set non_terminal_rhs_set = non_terminal.select_nodes("production");
            for (pugi::xpath_node_set::const_iterator rhs_it = non_terminal_rhs_set.begin(); rhs_it != non_terminal_rhs_set.end(); ++rhs_it)
            {
                pugi::xml_node rhs = (*rhs_it).node();
                pugi::xml_node symbol = rhs.first_child();
                bool rhs_nullable = true;
                // iterate through symbols in rhs
                do
                {
                    // if a non nullable symbol found
                    if (!this->nullable[symbol.name()])
                    {
                        rhs_nullable = false;
                        break;
                    }
                } while ((symbol = symbol.next_sibling()) != pugi::xml_node());
                // if all symbols in rhs are nullable
                if (rhs_nullable)
                {
                    this->nullable[non_terminal.name()] = true;
                    changed = true;
                    // std::cout << non_terminal.name() << " has rhs with all nullable symbols\n";
                    // break out of rhs iteration into non_terminal iteration
                    break;
                }
            }
        } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());
    }

    std::cout << "Nullable Values:\n";
    for (auto const &pair : this->nullable)
    {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}
bool node_is_terminal(pugi::xml_node &node)
{
    return node.attribute("terminal").as_bool() == true;
}
// bool Parser::is_terminal(const std::string &symbol)
// {
//     // in this context, terminals are symbols that do not have production rules defined
//     // we'll assume a terminal is a symbol not found in the CFG's non-terminals
//     pugi::xml_node productions = this->cfg_doc.child("CFG").child("PRODUCTIONRULES");
//     pugi::xml_node non_terminal = productions.first_child();

//     // check if the symbol is a non-terminal
//     do
//     {
//         if (symbol == non_terminal.name())
//         {
//             return false; // it's a non-terminal
//         }
//     } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());

//     return true; // if we didn't find it in non-terminals, it's a terminal
// }

void Parser::generate_first_sets()
{
    std::cout << "Generating FIRST sets\n";
    pugi::xml_node productions = this->cfg_doc.child("CFG").child("PRODUCTIONRULES");
    pugi::xml_node non_terminal = productions.first_child();

    // initialize FIRST sets
    do
    {
        this->first[non_terminal.name()] = std::set<std::string>(); // Using a set to avoid duplicates
    } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());

    bool changed = true;
    while (changed)
    {
        non_terminal = productions.first_child();
        changed = false;
        do
        {
            // iterate through each production for the non-terminal
            pugi::xpath_node_set non_terminal_rhs_set = non_terminal.select_nodes("production");
            for (pugi::xpath_node_set::const_iterator rhs_it = non_terminal_rhs_set.begin(); rhs_it != non_terminal_rhs_set.end(); ++rhs_it)
            {
                pugi::xml_node rhs = (*rhs_it).node();
                pugi::xml_node symbol = rhs.first_child();

                // iterate through symbols in RHS until a terminal or non-nullable non-terminal is found
                while (symbol != pugi::xml_node())
                {
                    std::string symbol_name = symbol.name();

                    // if the symbol is a terminal, add it to FIRST set
                    if (node_is_terminal(symbol))
                    {
                        if (symbol_name == "KEYWORD")
                        {
                            symbol_name = symbol.child("value").child_value();
                        }
                        else if (symbol_name != "EPSILON")
                        {
                            symbol_name = symbol.first_child().child_value();

                            switch (symbol_name[0])
                            {
                            case '"':
                                symbol_name = "T_Token";
                                break;
                            case 'V':
                                symbol_name = "V_Token";
                                break;
                            case 'F':
                                symbol_name = "F_Token";
                                break;
                            case '0':
                                symbol_name = "N_Token";
                            }
                        }
                        if (this->first[non_terminal.name()].insert(symbol_name).second) // If newly added
                        {
                            changed = true;
                        }
                        break;
                    }
                    else // it's a non-terminal
                    {
                        // add all non-epsilon FIRST symbols of the non-terminal to the current non-terminal
                        for (const std::string &first_symbol : this->first[symbol_name])
                        {
                            if (first_symbol != "EPSILON")
                            {
                                if (this->first[non_terminal.name()].insert(first_symbol).second)
                                {
                                    changed = true;
                                }
                            }
                        }

                        // if the non-terminal is not nullable, stop the loop
                        if (!this->nullable[symbol_name])
                        {
                            break;
                        }
                    }

                    symbol = symbol.next_sibling();
                }

                // if all symbols in RHS are nullable, add EPSILON to the FIRST set
                if (symbol == pugi::xml_node())
                {
                    if (this->first[non_terminal.name()].insert("EPSILON").second)
                    {
                        changed = true;
                    }
                }
            }
        } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());
    }

    std::cout << "FIRST Sets:\n";
    for (const auto &pair : this->first)
    {
        std::cout << pair.first << ": { ";
        for (const std::string &s : pair.second)
        {
            std::cout << "\"\033[33m" << s << "\033[0m\" ";
        }
        std::cout << "}\n";
    }
}

void Parser::generate_follow_sets()
{
    std::cout << "Generating FOLLOW sets\n";
    pugi::xml_node productions = this->cfg_doc.child("CFG").child("PRODUCTIONRULES");
    pugi::xml_node non_terminal = productions.first_child();

    // initialize FOLLOW sets with empty sets
    do
    {
        this->follow[non_terminal.name()] = std::set<std::string>();
    } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());

    // add the end of input marker ($) to the FOLLOW set of the start symbol (assumed to be the first non-terminal)
    std::string start_symbol = productions.first_child().name();
    this->follow[start_symbol].insert("$");

    bool changed = true;
    while (changed)
    {
        non_terminal = productions.first_child();
        changed = false;

        // iterate through all production rules
        do
        {
            std::string lhs = non_terminal.name();
            pugi::xpath_node_set non_terminal_rhs_set = non_terminal.select_nodes("production");

            for (pugi::xpath_node_set::const_iterator rhs_it = non_terminal_rhs_set.begin(); rhs_it != non_terminal_rhs_set.end(); ++rhs_it)
            {
                pugi::xml_node rhs = (*rhs_it).node();
                std::vector<pugi::xml_node> symbols;

                // collect all symbols in this production
                for (pugi::xml_node symbol = rhs.first_child(); symbol != pugi::xml_node(); symbol = symbol.next_sibling())
                {
                    symbols.push_back(symbol);
                }

                // now process the symbols to populate FOLLOW sets
                for (size_t i = 0; i < symbols.size(); ++i)
                {
                    pugi::xml_node B = symbols[i];

                    // only process non-terminals
                    if (node_is_terminal(B))
                    {
                        continue;
                    }
                    // case 2: A → αBβ (something comes after B)
                    if (i + 1 < symbols.size())
                    {
                        pugi::xml_node beta = symbols[i + 1];

                        // add FIRST(β) to FOLLOW(B), except EPSILON
                        if (node_is_terminal(beta))
                        {
                            std::string symbol_str = beta.name();
                            if (symbol_str == "KEYWORD")
                            {
                                symbol_str = beta.child("value").child_value();
                            }
                            else if (symbol_str != "EPSILON")
                            {
                                symbol_str = beta.first_child().child_value();

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
                            if (this->follow[B.name()].insert(symbol_str).second)
                            {
                                changed = true;
                            }
                        }
                        else
                        {
                            // add first set of β to FOLLOW(B)
                            for (const std::string &first_symbol : this->first[beta.name()])
                            {
                                if (first_symbol == "EPSILON")
                                {
                                    continue;
                                }
                                if (this->follow[B.name()].insert(first_symbol).second)
                                {
                                    changed = true;
                                }
                            }

                            // if β is nullable, add FOLLOW(β) to FOLLOW(B)
                            if (this->nullable[beta.name()])
                            {
                                for (const std::string &follow_symbol : this->follow[beta.name()])
                                {
                                    if (this->follow[B.name()].insert(follow_symbol).second)
                                    {
                                        changed = true;
                                    }
                                }
                            }
                        }
                    }
                    else // case 3: A → αB (B is at the end of the production)
                    {
                        // add FOLLOW(A) to FOLLOW(B)
                        for (const std::string &follow_symbol : this->follow[lhs])
                        {
                            if (this->follow[B.name()].insert(follow_symbol).second)
                            {
                                changed = true;
                            }
                        }
                    }
                }
            }
        } while ((non_terminal = non_terminal.next_sibling()) != pugi::xml_node());
    }

    std::cout << "FOLLOW Sets:\n";
    for (const auto &pair : this->follow)
    {
        std::cout << pair.first << ": { ";
        for (const std::string &s : pair.second)
        {
            std::cout << "\"\033[33m" << s << "\033[0m\" ";
        }
        std::cout << "}\n";
    }
}

std::shared_ptr<node> Parser::parse(std::string dest_name)
{
    std::unordered_map<int, std::unordered_map<std::string, std::pair<Operation, int>>> parse_table;
    std::unordered_map<std::string, int> nonterms;
    std::unordered_map<std::string, int> terms;
    Automaton production_automaton;
    pugi::xml_node productions = this->cfg_doc.child("CFG").child("PRODUCTIONRULES");
    production_automaton.cfg_to_nfa(productions);
    production_automaton.print_nfa();
    production_automaton.nfa_to_dfa();
    production_automaton.print_dfa();
    for (std::shared_ptr<State> s : production_automaton.dfa_states)
    {
        if (s->is_final && s->nfa_equiv_states.size() > 1)
        {
            std::vector<std::set<std::string>> follow_sets;
            std::cout << s->id << ": \n";
            for (auto nfas : s->nfa_equiv_states)
            {
                if (nfas.second->is_final)
                {
                    // std::cout << "    \033[33m" << nfas.first << "\033[0m ";
                    // std::cout << "[" << nfas.second->prod_num << "](\033[33m" << nfas.second->lhs_name << "\033[0m ==> ";
                    // for (auto n : nfas.second->rhs_nodes_list)
                    // {
                    //     std::cout << "\033[33m" << n << "\033[0m ";
                    // }
                    // std::cout << ") FOLLOW:";
                    std::set<std::string> curr_foll = this->follow[s->lhs_name];
                    follow_sets.push_back(curr_foll);

                    // auto it = curr_foll.begin();
                    // for (; it != curr_foll.end(); ++it)
                    // {
                    //     std::cout << " " << *it;
                    // }
                    // std::cout << "\n";
                }
                // else
                //     std::cout << "    " << nfas.first;
                // std::cout << std::endl;
            }
            for (auto nfas : s->nfa_equiv_states)
            {
                if (nfas.second->is_final)
                {
                    std::cout << "    \033[33m" << nfas.first << "\033[0m ";
                    std::cout << "[" << nfas.second->prod_num << "](\033[33m" << nfas.second->lhs_name << "\033[0m ==> ";
                    for (auto n : nfas.second->rhs_nodes_list)
                    {
                        std::cout << "\033[33m" << n << "\033[0m ";
                    }
                    std::cout << ") FOLLOW:";
                    std::set<std::string> curr_foll = this->follow[s->lhs_name];

                    auto it = curr_foll.begin();
                    for (; it != curr_foll.end(); ++it)
                    {
                        std::cout << " " << *it;
                    }
                    // std::cout << "\n";
                }
                // else
                std::cout << "    " << nfas.first;
                std::cout << std::endl;
            }
            if (follow_sets.size() > 1)
            {
                std::cout << "    Overlapping:\n";

                std::set<std::string> overlap;
                for (int i_curr = 0; i_curr < follow_sets.size() - 1; ++i_curr)
                {
                    for (auto symbol : follow_sets[i_curr])
                    {
                        std::cout << symbol << " ";
                        // if (overlap.find(symbol) == overlap.end())
                        // {
                        for (auto i_next = i_curr + 1; i_next != follow_sets.size(); ++i_next)
                        {

                            if (follow_sets[i_curr].find(symbol) != follow_sets[i_curr].end())
                            {
                                overlap.insert(symbol);
                                break;
                            }
                        }
                        // }
                    }
                    std::cout << std::endl;
                }
                if (overlap.size() > 0)
                {
                    std::cout << "    Overlap:";
                    for (auto it = overlap.begin(); it != overlap.end(); ++it)
                    {
                        std::cout << " " << *it;
                    }
                    std::cout << std::endl;
                }
            };
        }
    }
    pugi::xml_node lhs = productions.first_child();
    // // Print productions
    // do // while ((lhs = lhs.next_sibling()) != pugi::xml_node());
    // {
    //     pugi::xpath_node_set rhs_nodes = lhs.select_nodes("production");
    //     std::string lhs_name = lhs.name();
    //     // lhs_name = lhs_name;
    //     for (pugi::xpath_node_set::const_iterator rhs_it = rhs_nodes.begin(); rhs_it != rhs_nodes.end(); ++rhs_it)
    //     {
    //         for (int x = 0; x < 9 - lhs_name.length(); ++x)
    //         {
    //             std::cout << " ";
    //         }
    //         std::cout << lhs_name;
    //         std::cout << " ==>";
    //         pugi::xml_node node = (*rhs_it).node().first_child();
    //         std::string symbol_str;
    //         do // while ((node = node.next_sibling()) != pugi::xml_node());
    //         {
    //             symbol_str = node.name();
    //             std::string rhs_node_name = symbol_str;
    //             if (node.attribute("terminal").as_bool())
    //             {
    //                 if (symbol_str == "KEYWORD")
    //                 {
    //                     symbol_str = node.child("value").child_value();
    //                 }
    //                 else if (symbol_str != "EPSILON")
    //                 {
    //                     symbol_str = node.first_child().child_value();

    //                     switch (symbol_str[0])
    //                     {
    //                     case '"':
    //                         symbol_str = "T_Token";
    //                         break;
    //                     case 'V':
    //                         symbol_str = "V_Token";
    //                         break;
    //                     case 'F':
    //                         symbol_str = "F_Token";
    //                         break;
    //                     default:
    //                         symbol_str = "N_Token";
    //                     }
    //                 }
    //             }
    //             std::cout << " " << symbol_str;
    //         } while ((node = node.next_sibling()) != pugi::xml_node());
    //         std::cout << std::endl;
    //     }
    // } while ((lhs = lhs.next_sibling()) != pugi::xml_node());

    std::unordered_map<int, std::shared_ptr<State>> production_rules;
    for (std::shared_ptr<State> s : production_automaton.dfa_final_states)
    {
        production_rules[s->prod_num] = s;
        // std::cout << s;
        // std::cout << " [" << s->prod_num << "] : ";
        // std::cout << s->lhs_name << " ==>";
        // for (std::string n : s->rhs_nodes_list)
        // {
        //     std::cout << " " << n;
        // }
        // std::cout << std::endl;
    }
    // print production number and rules
    for (int i = 0; i < production_automaton.prod_num; ++i)
    {
        std::cout << production_rules[i] << std::endl;
        std::cout << " [" << production_rules[i]->prod_num << "] : " << std::endl;
        std::cout << production_rules[i]->lhs_name << " ==> ";
        for (std::string n : production_rules[i]->rhs_nodes_list)
        {
            std::cout << n << " ";
        }
        if (production_rules[i]->rhs_nodes_list.size() == 0 || production_rules[i]->lhs_name.length() == 0)
            std::cout << "(Empty state: " << production_rules[i]->id << ")";
        std::cout << std::endl;
    }
    for (auto state : production_automaton.dfa_states)
    {
        // std::cout << "Populating row " << state->id << std::endl;
        for (auto transition : state->transitions)
        {
            std::pair<Operation, int> op;
            std::string token = transition.first;
            // std::cout << "\t Symbol " << token;
            if (token.length() >= 2 && token[1] >= 65 && token[1] <= 90)
            {
                op.first = Operation::GO;
                nonterms[token] = std::max(nonterms[token], (int)token.length());
                nonterms[token] = std::max(nonterms[token], 6);
            }
            else
            {
                op.first = Operation::SHIFT;
                terms[token] = std::max(terms[token], (int)token.length());
                terms[token] = std::max(terms[token], 6);
            }
            // std::cout << ": " << ((char)op.first) << transition.second[0]->id << std::endl;
            op.second = transition.second[0]->id;
            parse_table[state->id][token] = op;
        }
        // std::pair<Operation, int> nonexist = parse_table[state->id]["asd"];
        // std::cout << "Nonexistent asd: {" << nonexist.first << ", " << nonexist.second << "}\n";
        // insert reduce actions
        if (state->is_final)
        {
            // std::cout << "\tState " << state->id << " accepts production " << state->prod_num << ": ";
            // std::cout << state->lhs_name << " ==>";
            // for (std::string n : state->rhs_nodes_list)
            // {
            //     std::cout << " " << n;
            // }
            // std::cout << "\n\tAdding Reduce Action on symbols: \"";
            // std::cout << "Token_Class " << state->lhs_name << "Prod_num " << state->prod_num << " Follow:\n";
            for (auto fol_sym : follow[state->lhs_name])
            {
                // std::cout << fol_sym << "\"";
                if (parse_table[state->id][fol_sym].first != 0)
                {
                    std::cout << "Collision at " << state->id << " on " << fol_sym << std::endl;
                    std::cout << "    Current action: " << (char)parse_table[state->id][fol_sym].first << parse_table[state->id][fol_sym].second << "\n";
                    std::cout << "    New     action: " << 'R' << state->prod_num << "\n";
                }
                parse_table[state->id][fol_sym] = {Operation::REDUCE, state->prod_num};
                terms[fol_sym] = std::max(2 + (int)(std::to_string(state->prod_num)).length(), terms[fol_sym]);
                if (fol_sym == "$" && state->prod_num == 1)
                {
                    parse_table[state->id][fol_sym] = {Operation::ACCEPT, 0};
                }

                // std::cout << "(" << ((char)parse_table[state->id][fol_sym].first) << parse_table[state->id][fol_sym].second << ") ";
            }
            // std::cout << std::endl;
        }
    }
    std::ofstream csv_file("parse-table.csv");
    // std::cout << std::setw(4) << "";
    csv_file << std::setw(4) << "State";
    for (auto sym : nonterms)
    {
        // std::cout << " | " << std::setw(sym.second) << std::left << sym.first;
        csv_file << " : " << std::setw(sym.second) << std::left << sym.first;
    }
    for (auto sym : terms)
    {
        // std::cout << " | " << std::setw(sym.second) << std::left << sym.first;
        csv_file << " : " << std::setw(sym.second) << std::left << sym.first;
    }
    // std::cout << std::endl;
    csv_file << std::endl;
    // for (auto row : parse_table)
    // {
    //     std::cout << row.first << ": \n";
    //     for (auto col : row.second)
    //     {
    //         std::cout << "\t" << col.first << ": " << ((char)(col.second.first)) << " " << col.second.second << "\n";
    //     }
    // }
    // print parse table to csv file
    for (auto state : parse_table)
    {
        // std::cout << state.first;
        csv_file << "S" << state.first;
        for (auto sym : nonterms)
        {
            auto action = state.second[sym.first];
            if (action.first != 0)
            {
                std::string temp = std::to_string(action.second);
                temp.insert(0, 1, (char)action.first);
                // std::cout << " | " << std::setw(sym.second) << std::left << temp;
                csv_file << " : " << std::setw(sym.second) << std::left << temp;
            }
            else
            {
                // std::cout << " | " << std::setw(sym.second) << std::left << "error";
                csv_file << " : " << std::setw(sym.second) << std::left << "error";
            }
        }
        for (auto sym : terms)
        {
            auto action = state.second[sym.first];
            if ((char)action.first != 0)
            {
                std::string temp = std::to_string(action.second);
                temp.insert(0, 1, action.first);
                // std::cout << " | " << std::setw(sym.second) << std::left << temp;
                csv_file << " : " << std::setw(sym.second) << std::left << temp;
            }
            else
            {
                // std::cout << " | " << std::setw(sym.second) << std::left << "error";
                csv_file << " : " << std::setw(sym.second) << std::left << "error";
            }
        }
        // std::cout << std::endl;
        csv_file << std::endl;
    }
    csv_file.close();

    // parse token stream
    pugi::xml_document tok_doc;
    pugi::xml_parse_result result = tok_doc.load_file("./token_stream.xml");
    if (!result)
    {
        throw "token_stream.xml does not exist";
    }
    pugi::xml_document ast_doc;
    pugi::xml_node tok_str = tok_doc.child("TOKENSTREAM");
    std::stack<std::pair<int, std::shared_ptr<node>>> parse_stack;
    parse_stack.emplace(std::pair(production_automaton.dfa_start_state->id, std::make_shared<node>()));
    bool accept = false;
    pugi::xml_node curr_tok = tok_str.first_child();
    int uid_counter = 0;
    while (!accept && curr_tok != pugi::xml_node())
    {
        { // debug printing
            std::cout << "Current stack:";
            std::stack<std::pair<int, std::shared_ptr<node>>> temp_stack;
            while (!parse_stack.empty())
            {
                temp_stack.emplace(parse_stack.top());
                // temp_stack.emplace(std::pair<Operation, std::shared_ptr<DummyNode>>(parse_stack.top().first, parse_stack.top().second));
                parse_stack.pop();
            }
            while (!temp_stack.empty())
            {
                std::cout << " " << temp_stack.top().first << "(" << temp_stack.top().second->WORD << ")";
                parse_stack.emplace(temp_stack.top());
                temp_stack.pop();
            }
            pugi::xml_node print_tok = curr_tok;
            // std::cout << "\nCurrent input:";
            // do
            // {
            //     std::cout << " " << print_tok.child("WORD").child_value();
            // } while ((print_tok = print_tok.next_sibling()) != pugi::xml_node());
            // std::cout << "\n";
        }
        std::pair<int, std::shared_ptr<node>> curr_state = parse_stack.top();
        std::string col_head = curr_tok.child("CLASS").child_value();
        if (col_head == "KEYWORD")
        {
            col_head = curr_tok.child("WORD").child_value();
        }
        else
        {
            std::cout << "\tNot \"KEYWORD\": \"" << col_head << "\"\n";
            std::cout << "\tLength (Expected " << (std::string("KEYWORD").length()) << ", Found" << col_head.length();
            col_head = curr_tok.child("CLASS").child_value();
        }
        std::pair<Operation, int> curr_op = parse_table[curr_state.first][col_head];
        std::cout << "parse_table[" << curr_state.first << "][" << col_head << "] = "
                  << ((char)curr_op.first) << curr_op.second << "\n";
        switch (curr_op.first)
        {
        case Operation::SHIFT:
        {
            std::cout << "Shifting new node: ";
            std::shared_ptr<node> new_node = std::make_shared<node>();
            new_node->NAME = "LEAF";
            new_node->UID = uid_counter++;
            new_node->CLASS = curr_tok.child("CLASS").child_value();
            new_node->WORD = curr_tok.child("WORD").child_value();

            std::cout << "UID=" << new_node->UID;
            std::cout << ", CLASS=" << new_node->CLASS;
            std::cout << ", WORD=" << new_node->WORD << "\n";

            curr_state = std::pair<int, std::shared_ptr<node>>(curr_op.second, new_node);
            parse_stack.emplace(curr_state);
            std::cout << "New parse stack element: " << parse_stack.top().second->NAME << "("
                      << "UID=" << parse_stack.top().second->UID
                      << ", CLASS=" << parse_stack.top().second->CLASS
                      << ", WORD=" << parse_stack.top().second->WORD << ")\n";
            curr_tok = curr_tok.next_sibling();
            break;
        }
        // Impossible, because only the case if a nonterminal is in the input stream, which is invalid
        case Operation::GO:
        {
            std::cout << "\n\n"
                      << curr_tok.child("WORD").child_value() << "\n\n";
            throw "Hey wtf, nonterminal in token stream";
            break;
        }

        case Operation::REDUCE:
        {
            std::string prod_LHS = production_rules[curr_op.second]->lhs_name;
            std::vector<std::string> prod_RHS = production_rules[curr_op.second]->rhs_nodes_list;
            std::cout << "REDUCTING [" << curr_op.second << "]: " << production_rules[curr_op.second]->lhs_name;
            std::cout << " ==>";
            for (auto n : production_rules[curr_op.second]->rhs_nodes_list)
            {
                std::cout << " " << n;
            }
            std::cout << "\n";
            std::shared_ptr<node> new_node = std::make_shared<node>();
            new_node->NAME = "INTERNAL";
            new_node->UID = uid_counter++;
            new_node->CLASS = "NONTERMINAL";
            new_node->WORD = prod_LHS.c_str();
            std::vector<std::string>::reverse_iterator rit = prod_RHS.rbegin();
            for (; rit != prod_RHS.rend(); ++rit)
            {
                curr_state = parse_stack.top();
                if (*rit != curr_state.second->WORD)
                {
                    std::cout << "Token word does not match, token class is: " << curr_state.second->CLASS << "\n";
                    if (curr_state.second->CLASS == "V_Token" && *rit == "VID")
                    {
                        pugi::xpath_node vid_node_expr = this->cfg_doc.select_node("TERMINALS/VID/pattern");
                        if (vid_node_expr != pugi::xpath_node())
                        {
                            std::string vid_str = "^";
                            vid_str.append(vid_node_expr.node().child_value());
                            vid_str.append("$");
                            std::cout << "Final regular expression: " << vid_str << "\n";
                            std::regex vid_regex(vid_str, std::regex_constants::ECMAScript);
                            if (std::regex_search(curr_state.second->WORD, vid_regex))
                            {
                                std::cout << "The token is a valid variable name\n";
                            }
                            else
                            {
                                std::cout << "The token is not a valid variable name\n";
                            }
                        }
                    }
                    else if (curr_state.second->CLASS == "F_Token" && *rit == "FID")
                    {
                        pugi::xpath_node fid_node_expr = this->cfg_doc.select_node("TERMINALS/FID/pattern");
                        if (fid_node_expr != pugi::xpath_node())
                        {
                            std::string fid_str = "^";
                            fid_str.append(fid_node_expr.node().child_value());
                            fid_str.append("$");
                            std::cout << "Final regular expression: " << fid_str << "\n";
                            std::regex fid_regex(fid_str, std::regex_constants::ECMAScript);
                            if (std::regex_search(curr_state.second->WORD, fid_regex))
                            {
                                std::cout << "The token is a valid function name\n";
                            }
                            else
                            {
                                std::cout << "The token is not a valid function name\n";
                            }
                        }
                    }
                    else if (*rit == "LITERAL" && (curr_state.second->CLASS == "N_Token" || curr_state.second->CLASS == "T_Token"))
                    {
                        pugi::xpath_node literal_node_expr = this->cfg_doc.select_node("TERMINALS/LITERAL");
                        if (literal_node_expr != pugi::xpath_node())
                        {
                            pugi::xml_node literal_pattern = literal_node_expr.node().first_child();
                            std::string literal_pattern_string = "^(";
                            literal_pattern_string.append(literal_pattern.child("pattern").child_value());
                            while ((literal_pattern = literal_pattern.next_sibling()) != pugi::xml_node())
                            {
                                literal_pattern_string.append("|");
                                literal_pattern_string.append(literal_pattern.child("pattern").child_value());
                            }
                            literal_pattern_string.append(")$");
                            std::regex decimal("\\.");
                            literal_pattern_string = std::regex_replace(literal_pattern_string, decimal, "\\\\.");
                            std::cout
                                << "Final regular expression: " << literal_pattern_string << "\n";
                            std::regex literal_regex(literal_pattern_string, std::regex_constants::ECMAScript);
                            if (std::regex_search(curr_state.second->WORD, literal_regex))
                            {
                                std::cout << "The token is a valid literal value\n";
                            }
                            else
                            {
                                std::cout << "The token is not a valid literal value\n";
                            }
                        }
                    }
                    else
                    {
                        std::cout << "\tTop of stack: {" << curr_state.first << ", " << curr_state.second->WORD
                                  << "} \n\tCurr Production Symbol: \"" << *rit << "\" Length " << rit->length();
                        std::cout << "\tExpected: \"" << curr_state.second->WORD << "\" Length " << curr_state.second->WORD.length() << "\n";
                        throw "\nInconsistent stack state\n";
                    }
                }
                new_node->get_children().insert(new_node->get_children().begin(), curr_state.second);

                parse_stack.pop();
            }
            curr_state = parse_stack.top();
            // should be a GO action
            std::pair<Operation, int> next_action = parse_table[curr_state.first][prod_LHS];
            if (next_action.first != Operation::GO)
            {
                std::cout << "\n\nReduced:  " << prod_LHS << " ==>";
                {
                    for (int i = 0; i < prod_RHS.size(); ++i)
                        std::cout << " " << prod_RHS[i];
                }
                std::cout << "\nNext Action from State " << curr_state.first
                          << ": " << ((char)next_action.first) << next_action.second;
                throw "\n\nERROR: Action After Reduce Must Be GO\n\n";
            }
            parse_stack.emplace(std::pair(next_action.second, new_node));
            break;
        }

        case Operation::ACCEPT:
        {
            accept = true;
            break;
        }
        default:
            std::cout << fmt::format("ERROR: NO ACTION FOR STATE {} ON INPUT SYMBOL {}.\n", curr_state.first, curr_tok.child("WORD").child_value());
            throw;
        }
    }
    if (accept)
    {
        // ast_doc.append_child("AST").append_copy(parse_stack.top().second);
        // if (ast_doc.save_file("./AST.xml"))
        // {
        //     std::cout << "\n\nSAVED AST TO AST.xml\n\n";
        // }
        // else
        // {
        //     std::cout << "\n\nSOMETHING WENT WRONG WHEN SAVING AST TO AST.xml\n\n";
        // }
        std::string tree = parse_stack.top().second->printnode(0, "");
        std::cout << "Parsed Syntax Tree";
        // std::cout << tree;
        std::ofstream ast(dest_name);
        ast << tree;
        ast.close();
        // parse_stack.top().second->print_code();
        std::cout << "\n";
        return parse_stack.top().second;
    }
    else
    {
        std::cout << "\n\nSomething went wrong\n\n";
    }
    throw;
}