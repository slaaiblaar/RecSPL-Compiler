
#include <fmt/core.h>
#include "testing.hpp"
#include "automata.hpp"
#include "pugixml.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <istream>
#include "ast_node.hpp"
#include <regex>
std::string Tester::print_code(std::shared_ptr<node> n)
{
    // prep for colour formatting
    for (auto e : this->scope_errors)
    {
        e.second->WORD = fmt::format("\033[31;1;4m{}\033[0m", e.first);
        e.second->print_colour = false;
    }
    for (auto e : this->discovered_scope_errors)
    {
        e.second->WORD = fmt::format("\033[37;9m{}\033[0m", e.first);
        e.second->print_colour = false;
    }
    std::ofstream dummy;
    std::string code = n->print_code(0, dummy, true);
    // dummy.close();
    // reset
    for (auto e : this->scope_errors)
    {
        e.second->WORD = e.first;
        e.second->print_colour = true;
    }
    for (auto e : this->discovered_scope_errors)
    {
        e.second->WORD = e.first;
        e.second->print_colour = true;
    }
    return code;
}
bool gen_automaton(std::string terminal_class, std::unordered_map<std::string, std::shared_ptr<Automaton>> &automata, std::string cfg_file = "CFG.xml")
{
    std::shared_ptr<Automaton> term_automaton = std::make_shared<Automaton>();
    pugi::xml_document doc;
    if (!doc.load_file(cfg_file.c_str()))
    {
        std::cerr << fmt::format("gen_automaton Error loading {}\n", cfg_file);
        automata[terminal_class] = nullptr;
        // throw;
        return false;
    }
    pugi::xml_node terminal = doc.child("CFG").child("TERMINALS").child(terminal_class.c_str()).first_child();
    do
    {
        term_automaton->append_pattern(terminal.child_value(), terminal.attribute("class").value());
    } while ((terminal = terminal.next_sibling()) != pugi::xml_node());
    term_automaton->nfa_to_dfa();
    automata[terminal_class] = term_automaton;
    return true;
}

void random_pattern(std::shared_ptr<node> n, std::string cfg_file = "CFG.xml")
{
    thread_local static std::unordered_map<std::string, std::shared_ptr<Automaton>> automata;
    // std::cout << "Generating pattern for: " << n->CLASS << " uid(" << n->UID << ")\n";
    thread_local static bool valid_automata = gen_automaton("VID", automata, cfg_file) && gen_automaton("FID", automata, cfg_file) && gen_automaton("LITERAL", automata, cfg_file);
    if (!valid_automata)
    {
        std::cerr << "\nA terminal pattern automaton was not initialised\n";
    }
    // 1% chance of incorrect pattern
    double mess_up = (((double)rand() / (double)RAND_MAX));
    // randomly construct a terminal pattern based on dfa built from regexp
    if (automata.find(n->CLASS) == automata.end())
    {
        std::cerr << "\nAn error occured while generating an automata for the terminal class " << n->CLASS << "\n";
        throw;
    }
    std::shared_ptr<Automaton> term_automaton = automata[n->CLASS];
    std::shared_ptr<State> dfa_state = automata[n->CLASS]->dfa_start_state;
    std::string pattern = "";
    // 20% chance of stopping at a final state, increases as pattern lengthens
    while (dfa_state != nullptr && !(dfa_state->is_final && (((double)rand() / (double)RAND_MAX) < (0.2 + (pattern.size() / 30.0)))))
    {
        if (dfa_state->transitions.size() == 0)
        {
            break;
        }
        std::vector<std::pair<std::string, std::vector<std::shared_ptr<State>>>> transitions;
        auto it = dfa_state->transitions.begin();
        for (; it != dfa_state->transitions.end(); ++it)
        {
            transitions.push_back(*it);
        }
        int random_transition = rand() % transitions.size();
        pattern.append(transitions[random_transition].first);
        dfa_state = transitions[random_transition].second[0];
    }
    if (dfa_state == nullptr)
    {
        std::cerr << "Something went wrong,  generated pattern " << pattern << "\n";
    }
    n->WORD = pattern;
}
/*
typedef struct
{
    int num_functions;
    int num_branches;
    int num_number_v;
    int num_text_v;
    int num_number_f;
    int num_void_f;
} counters;
*/
std::string counters_str(counters c, int depth)
{
    return fmt::format("[func: {}, branch: {}, v_num: {}, v_text: {}, f_num: {}, f_void: {}]",
                       c.num_functions,
                       c.num_branches,
                       c.num_number_v,
                       c.num_text_v,
                       c.num_number_f,
                       c.num_void_f);
}
int Tester::generate_tree(std::shared_ptr<node> parent, pugi::xml_node productions, int depth, component test, counters counters)
{

    if (parent->CLASS == "GLOBVARS" && depth > 10)
    {
        // emergency brake
        throw;
    }
    // reset lists
    if (parent->CLASS == "PROGPRIMEPRIME")
    {
        parent->scope_f_table = std::make_shared<ftable_type>();
        parent->scope_v_table = std::make_shared<vtable_type>();
    }
    // reset list of "sibling" var declarations
    if (parent->CLASS == "DECL")
    {
        parent->scope_v_table = std::make_shared<vtable_type>();
    }
    // reset list of "sibling" func declarations
    if (parent->CLASS == "SUBFUNCS")
    {
        parent->scope_f_table = std::make_shared<ftable_type>();
    }
    // configure tests, kind of
    bool stop_functions = counters.num_functions >= 3;
    bool force_branch = counters.num_branches < 3;
    bool force_var = (counters.num_number_v < 3 || counters.num_text_v < 3);
    bool force_func = (counters.num_number_f < 2 || counters.num_void_f < 2);
    // bool force_globvar = (globvar_counter < 2) && test == component::PARSER;
    // bool force_short_tree = false;
    // bool force_var = false;
    // bool force_func = false;
    // bool force_globvar = (counters.num_number_v < 0 && counters.num_text_v < 0);
    bool force_globvar = (counters.num_number_v + counters.num_text_v) < 3;
    bool force_short_tree = false;
    if (parent->CLASS == "BRANCH")
    {
        ++counters.num_branches;
    }
    if (parent->CLASS == "FUNCTIONS")
    {
        ++counters.num_functions;
    }
    if (parent->CLASS == "HEADER")
    {
        // function parameters are numbers
        counters.num_number_v += 3;
    }

    if (parent->subtree_generated)
    {
        return node::node_id_counter;
    }
    parent->subtree_generated = true;
    // for use in testing type checker
    thread_local static int node_counter = 0;
    thread_local static int globvar_counter = 0;
    // static int num_terminals = 0;
    if (depth == -1)
    {
        node_counter = 0;
        depth = 0;
        node::node_id_counter = 1;
        num_terminals = 0;
        globvar_counter = 0;
    }
    std::cout << fmt::format("{}generating subtree of {}, counters: {}\n", std::string(depth * 2, ' '), parent->CLASS, counters_str(counters, depth));
    // std::cout << std::string(depth, ' ') << parent->CLASS << "\n";
    // Find the production rule for the current node's WORD
    pugi::xpath_node lhs_xpath_node = productions.select_node(parent->CLASS.c_str());

    if (lhs_xpath_node != pugi::xpath_node())
    {
        // std::cout << std::string(depth, ' ') << parent->CLASS << " point NT point 1\n";
        // Non-terminal node
        pugi::xpath_node_set rhs_set = lhs_xpath_node.node().select_nodes("production");
        // if (rhs_set.size() > 0)
        // {
        // }
        parent->NAME = "INTERNAL";
        parent->WORD = parent->CLASS;
        // Randomly select one production rule (rhs)
        pugi::xml_node lhs = lhs_xpath_node.node();
        std::vector<pugi::xml_node> rhs_list;
        for (pugi::xml_node rhs = lhs.child("production"); rhs != pugi::xml_node(); rhs = rhs.next_sibling())
        {
            rhs_list.push_back(rhs);
        }
        // std::cout << std::string(depth, ' ') << parent->CLASS << " point NT point 2\n";
        // Select a random production (rhs) to expand
        pugi::xml_node rhs;
        if (parent->CLASS == "GLOBVARS")
        {
            ++globvar_counter;
        }
        int prod_num = -1;
        if (parent->WORD == "PROG")
        {
            if (force_var || force_globvar)
            {
                // std::cout << std::string(depth * 2, ' ') << "Adding GLOBVAR via PROG\n";
                prod_num = 0;
            }
            else if (force_short_tree)
            {
                prod_num = 1;
            }
        }
        // std::cout << std::string(depth, ' ') << parent->CLASS << fmt::format(" point NT point 3 force_var={} force_func={}\n", force_var, force_func);
        else if (parent->WORD == "GLOBVARS")
        {
            if (force_globvar)
            {
                // rhs = rhs_list[1];
                // std::cout << std::string(depth * 2, ' ') << "Adding GLOBVAR recursively\n";
                prod_num = 1;
                if (rhs_list[prod_num].child("GLOBVARS") == pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL FUNCTIONS\n";
                }
            }
            else
            {
                // rhs = rhs_list[0];
                prod_num = 0;
            }
        }
        else if (parent->WORD == "FUNCTIONS")
        {
            counters.num_number_v += 3; // locvars are always numbers
            if (force_func && !stop_functions)
            {
                // rhs = rhs_list[1];
                prod_num = 1;
                if (rhs_list[prod_num].child("FUNCTIONS") == pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL FUNCTIONS\n";
                }
            }
            else if (stop_functions || force_short_tree || !force_var)
            {
                // rhs = rhs_list[0];
                prod_num = 0;
                if (rhs_list[prod_num].child("FUNCTIONS") != pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL\n";
                }
            }
        }
        else if (parent->WORD == "INSTRUC")
        {
            // will result in about 4 instructions per function with 4 extra depth per scope block
            // PROGPRIMEPRIME => PROGPRIME => PROG => FUNCTIONS =>
            // with FUNCTIONS => DECL => BODY => ALGO => INSTRUC
            //          1          2       3      4         ^
            if ((depth - (3 + counters.num_functions * 4)) > 4)
            {
                // rhs = rhs_list[0];
                prod_num = 0;
                if (rhs_list[prod_num].child("INSTRUC") != pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of INSTRUC is not INSTRUC ==> COMMAND ;\n";
                }
            }
            else
            {
                prod_num = 1;
            }
        }
        else if (parent->WORD == "COND")
        {
            if ((depth - (3 + counters.num_functions * 4)) > 4)
            {
                prod_num = 0; // force SIMPLE cond
                if (rhs_list[prod_num].child("INSTRUC") != pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of INSTRUC is not INSTRUC ==> COMMAND ;\n";
                }
            }
        }
        else if (parent->WORD == "COMMAND")
        {
            do
            {
                // rhs = rhs_list[rand() % rhs_list.size()];
                prod_num = rand() % rhs_list.size();
            } while (rhs_list[prod_num].child("BRANCH") != pugi::xml_node());
        }
        else
        {
            // rhs = rhs_list[rand() % rhs_list.size()];
            prod_num = rand() % rhs_list.size();
        }
        if (prod_num == -1)
        {
            // std::cout << std::string(depth, ' ') << parent->CLASS << " point NT point 4\n";
            // rhs = rhs_list[rand() % rhs_list.size()];
            prod_num = rand() % rhs_list.size();
        }
        rhs = rhs_list[prod_num];
        if (parent->WORD == "GLOBVARS")
        {
            // std::cout << std::string(depth * 2, ' ') << parent->WORD << " ==>";
            // for (auto c : rhs)
            // {
            //     std::cout << " " << c.name();
            // }
            // std::cout << "\n";
        }
        parent->prod = prod_num;
        // std::cout << std::string(depth, ' ') << parent->CLASS << " point NT point 5\n";

        pugi::xml_node symbol = rhs.first_child();
        int child_index = 0;
        while (symbol != pugi::xml_node())
        {
            // std::cout << std::string(depth, ' ') << parent->CLASS << " point NT point 6\n";
            std::string sym_name = symbol.name();
            if (child_index != parent->children_size())
            {
                if (sym_name == "KEYWORD")
                {
                    sym_name = symbol.child("value").child_value();
                }
                std::cerr << fmt::format("\nchild_index={}, num children={}, symbol={}\n", child_index, parent->children_size(), sym_name);
                throw;
            }

            std::shared_ptr<node> child = std::make_shared<node>();
            child->CLASS = sym_name;
            child->scope_f_table = parent->scope_f_table;
            child->scope_v_table = parent->scope_v_table;
            generate_tree(child, productions, depth + 1, test, counters);

            std::cout << fmt::format("{}generated subtree of {}\n", std::string(depth * 2, ' '), child->WORD);
            bool is_new = parent->add_child(child, child_index);
            if (!is_new)
            {
                std::cerr << fmt::format("\n{}{} {} \"{}\" ATTEMPTED DUPLICATE CHILD OF {}\n", std::string(depth, ' '), child->CLASS, child->UID, child->WORD, parent->WORD);
                throw;
            }
            else
            {
                ++child_index;
                // ++num_terminals;
            }
            // pass down sibling list
            // child will create own if necessary
            // pugi::xpath_node term_xpath_node = productions.select_node(child->CLASS.c_str());
            if (symbol.attribute("terminal").as_bool()) // if terminal
            {
                // generate tree can't guess the correct keyword
                if (child->CLASS == "KEYWORD")
                {
                    child->WORD = symbol.child("value").child_value();
                    std::cout << fmt::format("{}  KEYWORD {}", std::string(depth * 2, ' '), child->WORD);
                    // 0.1 % chance of incorrect keyword
                    bool mess_up = (((double)rand() / (double)RAND_MAX) < 0.001);
                    if (test == component::LEXER && mess_up && this->messed_up_word.first < 0)
                    {
                        child->WORD = fmt::format("random_{}({})", std::to_string(rand() % 5), child->WORD);
                        this->messed_up_word = std::pair<int, std::string>(num_terminals, child->WORD);
                        std::cout << fmt::format("{}  Overwrite keyword to: {}", std::string(depth * 2, ' '), child->WORD);
                        // std::cout << fmt::format("{}Incorrect token generated: {}: \"{}\"\n", std::string(depth * 2, ' '), num_terminals, child->WORD);
                        // std::cout << child->printnode(depth, "generate_tree()");
                        child->was_printed = false;
                    }
                }
            }
            // rewrote CFG to have FTYP keywords be child of header directly
            if (parent->CLASS == "HEADER")
            {
                if (child->WORD == "num")
                {
                    ++counters.num_number_f;
                }
                if (child->WORD == "void")
                {
                    ++counters.num_void_f;
                }
            }
            if (child->CLASS == "VTYP")
            {
                std::cout << fmt::format("{}    VTYP->Child word: {}", std::string(depth * 2, ' '), child->get_child(0)->WORD);
                if (child->get_child(0)->WORD == "num")
                {
                    ++counters.num_number_v;
                }
                if (child->get_child(0)->WORD == "text")
                {
                    ++counters.num_text_v;
                }
            }
            // if (parent->WORD == "GLOBVARS")
            // {
            //     std::cout << std::string(depth * 2, ' ') << "Adding child " << child->WORD << "\n";
            // }
            symbol = symbol.next_sibling();
            // std::cout << std::string(depth * 2, ' ') << "Selected next symbol\n";
        }
    }
    else
    {
        parent->print_colour = true;
        ++num_terminals;
        // std::cout << std::string(depth, ' ') << parent->CLASS << " point T point 1\n";
        // Terminal node (VID, FID, KEYWORD, or LITERAL)
        parent->NAME = "LEAF";

        // Logic for terminal nodes (keywords, literals, or identifiers)
        if (parent->CLASS == "LITERAL")
        {
            random_pattern(parent, cfg_file);
        }
        else if (parent->CLASS != "KEYWORD")
        {
            parent->WORD = "PLACEHOLDER";
        }
    }
    // std::cout << std::string(depth, ' ') << parent->CLASS << " end\n";
    // std::cout << std::string(depth * 2, ' ') << "Completed generation for " << parent->WORD << "\n";
    return node::node_id_counter;
}

// making a type alias because its super annoying to repeatedly define objects
// of type std::shared_ptr<std::unordered_map<std::string, std::string>>
using ftable_type = std::unordered_map<std::string, std::string[4]>;
using vtable_type = std::unordered_map<std::string, std::string>;
// Drill down through successive chains of FUNCTIONS == > FUNCTIONS productions void node::copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)

std::shared_ptr<ftable_type> Tester::preprocess_ftables(std::shared_ptr<node> n, int depth)
{
    double collision_prob = 0.1;
    ++depth;
    std::shared_ptr<node> header = n->get_child(0)->get_child(0);
    if (header->WORD != "HEADER")
    {
        std::cerr << "\nFUNCTIONS->children[0]->children[0] is not a HEADER node\n";
    }
    std::shared_ptr<node> fname = header->get_child(1);
    if (fname->WORD != "FNAME")
    {
        std::cerr << "\nHEADER->children[1] is supposed to be a FNAME node\n";
    }
    std::shared_ptr<node> fid = fname->get_child(0);
    if (fid->CLASS != "FID")
    {
        std::cerr << "\nFNAME->children[1] is supposed to be a FID node\n";
    }
    double rand_collision = (double)rand() / (double)RAND_MAX;
    // if ()
    // generate name
    if (rand_collision > collision_prob || n->scope_f_table->size() == 0)
    {
        do
        {
            random_pattern(fid);
        } while (n->scope_f_table->find(fid->WORD) != n->scope_f_table->end());
        // bind type to name
        n->f_table[fid->WORD][0] = header->get_child(0)->WORD;
        (*n->scope_f_table)[fid->WORD][0] = header->get_child(0)->WORD;
        std::cout << fmt::format("{}Creating new function id {}\n", std::string(depth * 2, ' '), fid->WORD);
        std::cout << fmt::format("{}collision_prob: {}, rand_collision: {}, n->scope_f_table->size(): {} \n", std::string(depth * 2, ' '), collision_prob, rand_collision, n->scope_f_table->size());
        std::cout << fmt::format("{}(rand_collision > collision_prob || n->scope_f_table->size() == 0) : {} \n", std::string(depth * 2, ' '), (rand_collision > collision_prob || n->scope_f_table->size() == 0));
    }
    else
    {
        std::cout << fmt::format("{}Creating function scope collision error \n", std::string(depth * 2, ' '));
        std::cout << fmt::format("{}collision_prob: {}, rand_collision: {}, n->scope_f_table->size(): {} \n", std::string(depth * 2, ' '), collision_prob, rand_collision, n->scope_f_table->size());
        std::cout << fmt::format("{}(rand_collision > collision_prob || n->scope_f_table->size() == 0) : {} \n", std::string(depth * 2, ' '), (rand_collision > collision_prob || n->scope_f_table->size() == 0));
        int rand_id = rand() % n->scope_f_table->size();
        int index = 0;
        auto vit = n->scope_f_table->begin();
        while (vit != n->scope_f_table->end() && index < rand_id)
        {
            ++vit;
            ++index;
        }
        if (index == rand_id && vit != n->scope_f_table->end())
        {
            fid->WORD = vit->first;
            std::cout << fmt::format("{}Randomly selected duplicate fname: {} {}\n", std::string(depth * 2, ' '), fid->WORD, fid->UID);
            this->scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(fid->WORD, fid));
        }
    }

    std::shared_ptr<ftable_type> synthesized_table = std::make_shared<ftable_type>();
    (*synthesized_table)[fid->WORD][0] = header->get_child(0)->WORD;

    // check if a FUNCTIONS node is child
    bool prog_or_funcs = (n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->num_children() > 1 && n->get_child(n->num_children() - 1)->CLASS == "FUNCTIONS");
    if (prog_or_funcs && has_functions_child)
    {
        // copy current node's f_table to child
        std::shared_ptr<node> f_child = n->get_child(n->num_children() - 1);
        // node::copy_ftable(n, c);
        node::copy_ftable(n, f_child);
        std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
        // Copy child's f_table to current node
        node::copy_ftable(child_ftable, n);
        node::copy_ftable(child_ftable, synthesized_table);
    }
    n->pre_processed = true;
    return synthesized_table;
}
void Tester::construct_ftables(std::shared_ptr<node> n, int depth, component test)
{
    ++depth;
    thread_local static pugi::xml_document doc;
    thread_local static bool file_loaded = doc.load_file(cfg_file.c_str());
    if (!file_loaded)
    {
        // std::cout << "Error loading CFG.xml\n";
        std::cerr << fmt::format("construct_ftables Error loading {}\n", cfg_file);
        return;
    }
    std::shared_ptr<ftable_type> synthesized = std::make_shared<ftable_type>();
    thread_local static pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");
    thread_local static pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
    if (terminals.child(n->CLASS.c_str()) != pugi::xml_node())
    {
        return;
    }
    bool prog_or_funcs = (n->CLASS == "PROG" || n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->children_size() > 1 && n->get_child(n->children_size() - 1)->CLASS == "FUNCTIONS");
    // if no functions that affect other nodes in scope
    bool preprocessing_required = false;
    int f_index = -1;
    if (prog_or_funcs && has_functions_child)
    {
        preprocessing_required = true;
        f_index = n->children_size() - 1;
    }

    bool body_node = (n->CLASS == "BODY");
    bool has_dubfuncs_child = (n->children_size() > 2 && n->get_child(n->children_size() - 2)->CLASS == "SUBFUNCS");
    if (body_node && has_dubfuncs_child)
    {
        preprocessing_required = true;
        f_index = n->children_size() - 2;
    }
    // preprocess function descendents
    if (preprocessing_required && !n->pre_processed)
    {
        n->pre_processed = true;
        std::shared_ptr<node> f_child = n->get_child(f_index);
        // get FUNCTIONS child of SUBFUNCS if necessary
        if (f_child->CLASS == "SUBFUNCS")
        {
            f_child = f_child->get_child(0);
        }
        node::copy_ftable(n, f_child);

        std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
        // Copy child's f_table
        node::copy_ftable(f_child, n);
        // copy to SUBFUNCS if applicable
        if (n->get_child(f_index)->UID != f_child->UID)
        {
            f_child = n->get_child(f_index);
            node::copy_ftable(n, f_child);
        }
    }
    for (auto c : n->get_children())
    {
        for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
        {
            for (int i = 0; i < 4; ++i)
            {
                c->f_table[it->first][i] = it->second[i];
            } // popagate f_tables
        }
        construct_ftables(c, depth + 1, test);
    }
    return;
}
void Tester::populate_identifiers(std::shared_ptr<node> n, component test)
{
    double collision_prob = 0.1;
    double invalid_pattern_prob = 0.05;
    thread_local static int depth = -1;
    ++depth;
    if (n->CLASS == "GLOBVARS" || n->CLASS == "LOCVARS" || n->CLASS == "HEADER" || n->CLASS == "COMMAND")
    {
        // std::cout << fmt::format("{}Populating ids of: {}\n", std::string(depth * 2, ' '), n->CLASS);
    }
    // std::string indentation = std::string(depth * 2, ' ');
    // std::cout << fmt::format("CLASS: {}\n", n->CLASS);
    if (n->children_size() == 0)
    {
        ++this->num_terminals;
    }
    if ((n->CLASS == "VID" || n->CLASS == "FID") && n->WORD == "PLACEHOLDER")
    {
        // 95% of selecting a legitimate ID
        double id_probability = (double)rand() / (double)RAND_MAX;
        if (n->CLASS == "VID" && n->v_table.size() == 0)
        {
            id_probability = 0;
        }
        if (n->CLASS == "FID" && n->f_table.size() == 0)
        {
            id_probability = 0;
        }
        if (id_probability > invalid_pattern_prob)
        {
            // std::cout << fmt::format("{}  Legitimate id\n", std::string(depth * 2, ' '));
            // std::cout << fmt::format("  {} existing id\n", n->CLASS);
            int rand_id = 0;
            int index = 0;
            if (n->CLASS == "VID")
            {
                rand_id = rand() % n->v_table.size();
                // std::cout << fmt::format("{}  Random Variable ID: {}\n", std::string(depth * 2, ' '), rand_id);
                auto vit = n->v_table.begin();
                while (index < rand_id && vit != n->v_table.end())
                {
                    ++index;
                    ++vit;
                }
                if (index == rand_id && vit != n->v_table.end())
                {
                    n->WORD = vit->first;
                }
            }
            else if (n->CLASS == "FID")
            {
                rand_id = rand() % n->f_table.size();
                // std::cout << fmt::format("{}  Random Function ID: {}\n", std::string(depth * 2, ' '), rand_id);
                auto fit = n->f_table.begin();
                while (index < rand_id && fit != n->f_table.end())
                {
                    ++index;
                    ++fit;
                }
                if (index == rand_id && fit != n->f_table.end())
                {
                    n->WORD = fit->first;
                }
            }
        }
        // will skip loop and choose random id if none are in scope
        // 5% chance of identifier being random string if none exists
        if (test == component::LEXER && this->messed_up_word.first < 0 && id_probability <= invalid_pattern_prob)
        {
            std::cout << fmt::format("{}  Invalid id\n", std::string(depth * 2, ' '), n->WORD);
            // random invalid identifier
            if (n->CLASS == "VID")
            {
                n->WORD = fmt::format("rand_VID_{}", rand() % 99);
            }
            else if (n->CLASS == "FID")
            {
                n->WORD = fmt::format("rand_FID_{}", rand() % 99);
            }
            this->messed_up_word = std::pair<int, std::string>(this->num_terminals, n->WORD);
        }
        else if (n->WORD == "PLACEHOLDER")
        {
            random_pattern(n);
        }

        if (n->CLASS == "VID")
        {
            n->is_in_scope = n->v_table.find(n->WORD) != n->v_table.end();
        }
        else if (n->CLASS == "FID")
        {
            n->is_in_scope = n->f_table.find(n->WORD) != n->f_table.end();
        }
        // if (!n->is_in_scope)
        // {
        //     std::cout << fmt::format("{}  {} Not in scope\n", std::string(depth * 2, ' '), n->WORD);
        // }

        if (!n->is_in_scope && test == component::SCOPE_CHECK)
        {
            // this->messed_up_word = std::pair<int, std::string>(n->UID, n->WORD);
            scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(n->WORD, n));
        }
        return;
    } // end if ((n->CLASS == "VID" || n->CLASS == "FID") && n->WORD == "PLACEHOLDER")
    double id_probability;
    if (n->CLASS == "GLOBVARS")
    {
        std::shared_ptr<node> vtype = n->get_child(0);
        std::shared_ptr<node> vname = n->get_child(1);
        id_probability = (double)rand() / (double)RAND_MAX;

        // make name identical to a randomly selected name in the current v_table
        // the selected name is guaranteed to be another globvar, because
        // globvars appear first in a program
        if (test == component::SCOPE_CHECK && id_probability < collision_prob && n->scope_v_table->size() > 0)
        {
            std::cout << fmt::format("{}  Creating globvar scope collision error \n", std::string(depth * 2, ' '));
            int rand_id = rand() % n->scope_v_table->size();
            int index = 0;
            auto vit = n->scope_v_table->begin();
            while (vit != n->scope_v_table->end() && index < rand_id)
            {
                ++vit;
                ++index;
            }
            if (index == rand_id && vit != n->scope_v_table->end())
            {
                vname->get_child(0)->WORD = vit->first;
                // std::cout << fmt::format("{}Randomly selected duplicate vname: {} {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, vname->get_child(0)->UID);
                this->scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(vname->get_child(0)->WORD, vname->get_child(0)));
            }
        }
        else
        {
            // std::cout << fmt::format("{}  Creating ordinary globvar \n", std::string(depth * 2, ' '));
            do
            {
                random_pattern(vname->get_child(0));
                // std::cout << fmt::format("{}  Random vname: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD);

                // random_pattern(vname->get_child(0));
                // while the name is not unique
            } while (n->scope_v_table->find(vname->get_child(0)->WORD) != n->scope_v_table->end());
            n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
            (*n->scope_v_table)[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
        }
        // std::cout << fmt::format("{}GLOBVAR created \n", std::string(depth * 2, ' '));
        // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
    }
    // LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
    //              0     1   2   3    4   5  6     7   8
    if (n->CLASS == "LOCVARS")
    {
        // std::set<std::string> locvars;
        // auto n1 = n->get_child(4)->get_child(0);
        // std::vector<std::pair<std::shared_ptr<node>, std::shared_ptr<node>>> vars;
        // for (int i = 0; i < 7; i += 3)
        // {
        //     std::shared_ptr<node> vtype = n->get_child(i);     // 0, 3, 6
        //     std::shared_ptr<node> vname = n->get_child(i + 1); // 1, 4, 7
        //     bool found = false;
        //     do
        //     {
        //         random_pattern(vname->get_child(0));
        //         // while the name is not unique
        //     } while (n->scope_v_table->find(vname->get_child(0)->WORD) != n->scope_v_table->end());
        //     vars.push_back(std::pair<std::shared_ptr<node>, std::shared_ptr<node>>(vtype->get_child(0), vname->get_child(0)));
        //     // locvars.emplace(vname->get_child(0)->WORD);
        //     // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
        // }
        // id_probability = (double)rand() / (double)RAND_MAX;
        // for (auto v : vars)
        // {
        //     if (test == component::SCOPE_CHECK && id_probability < 1 && n->scope_v_table->size() > 0)
        //     {
        //         int rand_id = rand() % n->scope_v_table->size();
        //         auto vit = n->scope_v_table->begin();
        //         int index = 0;
        //         while (vit != n->v_table.end() && index < rand_id)
        //         {
        //             ++vit;
        //             ++index;
        //         }
        //         if (index == rand_id && vit != n->scope_v_table->end())
        //         {
        //             v.second->WORD = vit->first;
        //             std::cout << fmt::format("{}Randomly selected duplicate LOCVAR: {} {}\n", std::string(depth * 2, ' '), v.second->WORD, v.second->UID);
        //             this->scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(v.second->WORD, v.second));
        //             // don't need to touch scope tables
        //             // because it is a duplicate and technically in scope
        //             continue;
        //         }
        //         n->v_table[v.first->WORD] = v.second->WORD;
        //         n->v_id_map[v.first->WORD] = v.second->WORD;
        //     }
        // }
        std::set<std::string> locvars;
        auto n1 = n->get_child(4)->get_child(0);
        std::vector<std::pair<std::shared_ptr<node>, std::shared_ptr<node>>> vars;
        for (int i = 0; i < 7; i += 3)
        {
            std::shared_ptr<node> vtype = n->get_child(i);     // 0, 3, 6
            std::shared_ptr<node> vname = n->get_child(i + 1); // 1, 4, 7
            id_probability = (double)rand() / (double)RAND_MAX;
            int dup_index = -1;
            if (id_probability < collision_prob)
            {
                dup_index = rand() % 3;
            }
            int curr_locvar = 0;
            // make name identical to a randomly selected name in the current v_table
            // the selected name is guaranteed to be another globvar, because
            // globvars appear first in a program
            if (test == component::SCOPE_CHECK && dup_index == curr_locvar && n->v_table.size() > 0)
            {
                // std::cout << fmt::format("{}  Creating locvar scope collision error \n", std::string(depth * 2, ' '));
                int rand_id = rand() % n->scope_v_table->size();
                int index = 0;
                auto vit = n->scope_v_table->begin();
                while (vit != n->scope_v_table->end() && index < rand_id)
                {
                    ++vit;
                    ++index;
                }
                if (index == rand_id && vit != n->scope_v_table->end())
                {
                    vname->get_child(0)->WORD = vit->first;
                    // std::cout << fmt::format("{}Randomly selected duplicate vname: {} {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, vname->get_child(0)->UID);
                    this->scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(vname->get_child(0)->WORD, vname->get_child(0)));
                }
            }
            else
            {
                // std::cout << fmt::format("{}  Creating ordinary locvar \n", std::string(depth * 2, ' '));
                do
                {
                    random_pattern(vname->get_child(0));
                    // std::cout << fmt::format("{}  Random vname: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD);

                    // random_pattern(vname->get_child(0));
                    // while the name is not unique
                } while (n->scope_v_table->find(vname->get_child(0)->WORD) != n->scope_v_table->end());
                n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
                (*n->scope_v_table)[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
            }
            // std::cout << fmt::format("{}LOCVAR created \n", std::string(depth * 2, ' '));
        }
    }
    // n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;

    // HEADER ==> ftyp FNAME ( VNAME , VNAME , VNAME )
    //              0     1   2   3   4   5   6   7   8
    if (n->CLASS == "HEADER")
    {
        // std::set<std::string> params;
        // for (int i = 3; i < 8; i += 2)
        // {
        //     std::shared_ptr<node> vname = n->get_child(i); // 3, 5, 7
        //     bool found = false;
        //     do
        //     {
        //         id_probability = (double)rand() / (double)RAND_MAX;
        //         if (test == component::SCOPE_CHECK && id_probability < 1 && params.size() > 0)
        //         {

        //             int rand_id = rand() % params.size();
        //             int index = 0;
        //             for (auto vit = params.begin(); vit != params.end() && index < rand_id; ++index)
        //             {
        //                 if (index == rand_id)
        //                 {
        //                     vname->get_child(0)->WORD = *vit;
        //                     found = true;
        //                     this->scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(vname->get_child(0)->WORD, vname->get_child(0)));
        //                     break;
        //                 }
        //             }
        //         }
        //         else
        //         {
        //             random_pattern(vname->get_child(0));
        //         }
        //         random_pattern(vname->get_child(0));
        //         // while the name is not unique
        //         // not necessary but makes manual verification easier
        //     } while (n->v_table.find(vname->get_child(0)->WORD) != n->v_table.end());
        //     n->v_table[vname->get_child(0)->WORD] = "num";
        //     // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
        // }
        std::set<std::string> params;
        for (int i = 3; i < 8; i += 2)
        {
            std::shared_ptr<node> vname = n->get_child(i); // 3, 5, 7
            id_probability = (double)rand() / (double)RAND_MAX;
            int dup_index = -1;
            if (id_probability < collision_prob)
            {
                dup_index = rand() % 3;
            }
            int curr_locvar = 0;
            // make name identical to a randomly selected name in the current v_table
            // the selected name is guaranteed to be another globvar, because
            // globvars appear first in a program
            if (test == component::SCOPE_CHECK && dup_index == curr_locvar && n->scope_v_table->size() > 0)
            {
                // std::cout << fmt::format("{}  Creating parameter scope collision error \n", std::string(depth * 2, ' '));
                int rand_id = rand() % n->scope_v_table->size();
                int index = 0;
                auto vit = n->scope_v_table->begin();
                while (vit != n->scope_v_table->end() && index < rand_id)
                {
                    ++vit;
                    ++index;
                }
                if (index == rand_id && vit != n->scope_v_table->end())
                {
                    vname->get_child(0)->WORD = vit->first;
                    // std::cout << fmt::format("{}Randomly selected duplicate vname: {} {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, vname->get_child(0)->UID);
                    this->scope_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(vname->get_child(0)->WORD, vname->get_child(0)));
                }
            }
            else
            {
                // std::cout << fmt::format("{}  Creating ordinary parameter \n", std::string(depth * 2, ' '));
                do
                {
                    random_pattern(vname->get_child(0));
                    // std::cout << fmt::format("{}  Random vname: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD);
                    // random_pattern(vname->get_child(0));
                    // while the name is not unique
                } while (n->scope_v_table->find(vname->get_child(0)->WORD) != n->scope_v_table->end());
                n->v_table[vname->get_child(0)->WORD] = "num";
                (*n->scope_v_table)[vname->get_child(0)->WORD] = "num";
            }
            // std::cout << fmt::format("{}Parameter created \n", std::string(depth * 2, ' '));
        }
    }
    for (auto c : n->get_children())
    {
        node::copy_vtable(n, c);
        node::copy_ftable(n, c);
        populate_identifiers(c, test);
        if (c->CLASS == "GLOBVARS" || c->CLASS == "LOCVARS" || c->CLASS == "HEADER")
        {
            node::copy_vtable(c, n);
            // std::cout << fmt::format("  {}: Copied vtable from child {}\n", n->CLASS, c->CLASS);
        }
    }
    --depth;
}
bool compare_nodes(std::shared_ptr<node> l, std::shared_ptr<node> r, bool debug = false)
{
    if (debug)
        std::cout << fmt::format("(C:W:N): L ({}:{}:{}) , R ({}:{}:{})\n", l->CLASS, l->WORD, l->NAME, r->CLASS, r->WORD, r->NAME);

    if (debug && (l->WORD != r->WORD /*|| l->CLASS != r->CLASS */))
    {
        std::cout << "NOT THE SAME\n";
        return false;
    }
    auto l_children = l->get_children();
    auto r_children = r->get_children();
    if (l_children.size() > 0 || r_children.size() > 0)
    {
        if (debug)
        {
            std::cout << "Comparing children\nL: ";
            for (auto c : l_children)
            {
                std::cout << " " << c->WORD;
            }
            std::cout << "\nR: ";
            for (auto c : r_children)
            {
                std::cout << " " << c->WORD;
            }
            std::cout << "\n";
        }
        auto l_it = l_children.begin();
        auto r_it = r_children.begin();
        // std::cout << fmt::format("Num children: L:{}, R:{}\n", l_children.size(), r_children.size());
        if (l_children.size() != r_children.size())
        {
            if (debug)
                std::cout << "NOT THE SAME\n";
            return false;
        }
        for (; r_it != r_children.end() && l_it != l_children.end(); ++r_it, ++l_it)
        {
            if (!compare_nodes(*l_it, *r_it))
            {
                return false;
            }
        }
    }
    return true;
}

void Tester::run_tests(int thread_number)
{
    std::vector<std::string> results;
    test_lexer(thread_number, results);
    test_parser(thread_number, results);
    test_scope_checker(thread_number, results);
    // test_type_checker();
    // test_code_generator();
    for (auto r : results)
    {
        std::cout << r;
    }
    std::cout << fmt::format("\033[3{}mThread {}\033[0m testing completed\n", 2 + thread_number, thread_number);
}

void Tester::test_lexer(int thread_number, std::vector<std::string> &results)
{
    std::cout << "===== Running Random Program Test =====\n";
    // this->cfg_file = fmt::format("CFG{}.xml", thread_number);
    pugi::xml_document doc;
    if (!doc.load_file(cfg_file.c_str()))
    {
        std::cerr << fmt::format("run_tests Error loading {}\n", cfg_file);
        return;
    }

    pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");

    // root node for tree
    std::shared_ptr<node> root = std::make_shared<node>();
    // std::time_t now = std::time(NULL);
    // srand(now);
    std::time_t now = std::time(0);
    srand(now);
    int num_nodes = 0;
    Lexer l(this->cfg_file);
    bool bad_lex = false;
    for (int i = 0; !bad_lex && i < 40; ++i)
    {
        std::shared_ptr<node> root = std::make_shared<node>();
        srand(rand());
        int num_nodes = 0;

        { // do
            this->messed_up_word = std::pair<int, std::string>(-1, "NONE");
            root->clear_node();
            root->CLASS = "PROGPRIMEPRIME";
            num_nodes = this->generate_tree(root, productions, -1, component::LEXER);
        } // while (num_nodes < 300);

        std::ofstream rand_tree;
        std::ofstream code_file;

        construct_ftables(root, 0, component::LEXER);

        this->num_terminals = 0;
        populate_identifiers(root, component::LEXER);
        std::string tree = root->printnode(0, "testScopeChecker()");
        tree = root->printnode(0, "testScopeChecker()");
        code_file.open(fmt::format("thread_{}_code_file.txt", thread_number));
        std::string plaintext_code = root->print_code(0, code_file);
        code_file << plaintext_code;
        code_file.close();
        // // do something with lexer
        l.read_input(fmt::format("./thread_{}_code_file.txt", thread_number));
        bool lex_res = l.lex(true, fmt::format("./thread_{}_token_stream.xml", thread_number));
        pugi::xml_document tok_doc;
        pugi::xml_parse_result result = tok_doc.load_file(fmt::format("./thread_{}_token_stream.xml", thread_number).c_str());
        pugi::xml_node tok_str = tok_doc.child("TOKENSTREAM");
        pugi::xml_node curr_tok = tok_str.first_child();
        int tok_counter = 0;
        while (curr_tok != pugi::xml_node())
        {
            ++tok_counter;
            curr_tok = curr_tok.next_sibling();
        }
        bool incorrect_pass = this->messed_up_word.first > 0 && lex_res && l.dfa.input.find(this->messed_up_word.second) != std::string::npos;
        // std::vector<std::string> lex_results;
        if (this->messed_up_word.first > 0 && !lex_res)
        {
            results.push_back(fmt::format("\033[3{}m{}\033[0m:   \033[32mSuccessfully\033[0m failed lexing\n\tNumber of tokens lexed:{}, Incorrect token: {}: \"{}\"\n", 2 + thread_number, thread_number, tok_counter, this->messed_up_word.first, this->messed_up_word.second));
            results.push_back(fmt::format("\033[3{}m{}\033[0m\tLexer error message: \"{}\"\n\n", 2 + thread_number, thread_number, l.message));
        }
        else if (this->messed_up_word.first <= 0 && !lex_res)
        {
            std::cout << "REALLY SHOULDN'T BE HERE\n";
            results.push_back(fmt::format("\033[3{}m{}\033[0m: \033[31mUnsuccessfully\033[0m failed when it should have passed\n\tNumber of tokens lexed:{}\n", 2 + thread_number, thread_number, tok_counter));
            results.push_back(fmt::format("\033[3{}m{}\033[0m\tLexer error message: \"{}\"\n\n", 2 + thread_number, thread_number, l.message));
            l.dfa.print_dfa(fmt::format("failed_test_artefacts/thread_{}_render-dfa.txt", thread_number), fmt::format("failed_test_artefacts/thread_{}_dfa_graph_incorrectly_failed.txt", thread_number));
            l.print_tokens(fmt::format("failed_test_artefacts/thread_{}_token_stream_incorrectly_failed.xml", thread_number));

            code_file.open(fmt::format("failed_test_artefacts/thread_{}_code_file_incorrectly_failed.txt", thread_number));
            std::string plaintext_code = root->print_code(0, code_file);
            code_file << plaintext_code;

            code_file.close();
            rand_tree.open(fmt::format("failed_test_artefacts/thread_{}_rand_tree_incorrectly_failed.xml", thread_number));
            rand_tree << root->printnode(0, "TESTING");
            rand_tree.close();
            bad_lex = true;
        }
        else if (incorrect_pass)
        { // fmt::format("{}: Parser \033[32mPassed\033[0m\n", i)
            results.push_back(fmt::format("\033[3{}m{}\033[0m: \033[31Unsuccessfully\033[0m passed when it should have failed\n", 2 + thread_number, thread_number));
            results.push_back(fmt::format("\033[3{}m{}\033[0m\tNumber of tokens lexed:{}, Incorrect token: {}: \"{}\"\n\n", 2 + thread_number, thread_number, tok_counter, this->messed_up_word.first, this->messed_up_word.second));
            l.dfa.print_dfa(fmt::format("failed_test_artefacts/thread_{}_render-dfa.txt", thread_number), fmt::format("failed_test_artefacts/thread_{}_dfa_graph_incorrectly_passed.txt", thread_number));
            l.print_tokens(fmt::format("failed_test_artefacts/thread_{}_token_stream_incorrectly_passed.xml", thread_number));

            std::string plaintext_code = root->print_code(0, code_file);
            code_file.open(fmt::format("failed_test_artefacts/thread_{}_code_file_incorrectly_passed.txt", thread_number));
            code_file << plaintext_code;

            code_file.close();
            code_file.close();
            rand_tree.open(fmt::format("failed_test_artefacts/thread_{}_rand_tree_incorrectly_passed.xml", thread_number));
            rand_tree << root->printnode(0, "TESTING");
            rand_tree.close();
            bad_lex = true;
        }
        else if (this->messed_up_word.first <= 0 && lex_res)
        {
            results.push_back(fmt::format("\033[3{}m{}\033[0m:   \033[32mSuccessfully\033[0m passed lexing\n\tNumber of tokens lexed:{}, number of generated terminals: {}\n\n", 2 + thread_number, thread_number, tok_counter, this->num_terminals));
        }
        // for (auto lr : results)
        // {
        //     std::cout << lr;
        // }
        root->clear_node();
    }
    std::cout << fmt::format("\033[3{}mThread {}\033[0m testing completed\n", 2 + thread_number, thread_number);
}
void Tester::test_parser(int thread_number, std::vector<std::string> &results)
{

    std::cout << "===== Running Random Program Test =====\n";
    // this->cfg_file = fmt::format("CFG{}.xml", thread_number);
    pugi::xml_document doc;
    if (!doc.load_file(cfg_file.c_str()))
    {
        std::cerr << fmt::format("run_tests Error loading {}\n", cfg_file);
        return;
    }

    pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");

    // root node for tree
    std::shared_ptr<node> root = std::make_shared<node>();
    // std::time_t now = std::time(NULL);
    // srand(now);
    std::time_t now = std::time(0);
    srand(now);
    int num_nodes = 0;
    Lexer l(this->cfg_file);
    bool bad_parse = false;
    for (int i = 0; !bad_parse && i < 40; ++i)
    {
        std::shared_ptr<node> root = std::make_shared<node>();
        srand(rand());
        int num_nodes = 0;

        do
        {
            this->messed_up_word = std::pair<int, std::string>(-1, "NONE");
            root->clear_node();
            root->CLASS = "PROGPRIMEPRIME";
            num_nodes = this->generate_tree(root, productions, -1, component::PARSER);
        } while (num_nodes < 300);

        std::ofstream rand_tree;
        std::ofstream code_file;

        construct_ftables(root, 0, component::PARSER);

        this->num_terminals = 0;
        populate_identifiers(root, component::PARSER);
        std::string tree = root->printnode(0, "testScopeChecker()");
        tree = root->printnode(0, "testScopeChecker()");
        std::ofstream tree_file(fmt::format("thread_{}_generated_ast.xml", thread_number));
        tree_file << tree;
        tree_file.close();
        code_file.open(fmt::format("thread_{}_code_file.txt", thread_number));
        std::string plaintext_code = root->print_code(0, code_file);
        code_file << plaintext_code;
        code_file.close();
        // // do something with lexer
        l.read_input(fmt::format("./thread_{}_code_file.txt", thread_number));
        bool lex_res = l.lex(true, fmt::format("./thread_{}_token_stream.xml", thread_number));
        if (!lex_res)
        {
            std::cout << "Lexing failed\n";
            return;
        }
        Parser p(this->cfg_file);
        std::shared_ptr<node> parsed_root = p.parse(fmt::format("./thread_{}_parsed_ast.xml", thread_number), fmt::format("./thread_{}_token_stream.xml", thread_number));
        // code_file.open(fmt::format("thread_{}_generated_code_file.txt", thread_number));
        // plaintext_code = parsed_root->print_code(0, code_file);
        // code_file << plaintext_code;
        // code_file.close();
        // root is PROGPRIMEPRIME
        std::shared_ptr<node> generated_root = root->get_child(0)->get_child(0);
        if (compare_nodes(generated_root, parsed_root))
        {
            // std::cout << i << ": Parser succeeded\n";
            results.push_back(fmt::format("{}: Parser \033[32mPassed\033[0m\n", i));
        }
        else
        {
            results.push_back(fmt::format("{}: Parser \033[32mFailed\033[0m\n", i));
            // std::cout << i << ": Parser failed\n";
            bad_parse = true;
        }
        root->clear_node();
    }
    std::cout << fmt::format("\033[3{}mThread {}\033[0m testing completed\n", 2 + thread_number, thread_number);
}

void Tester::test_scope_checker(int thread_number, std::vector<std::string> &results)
{

    std::cout << "===== Running Random Program Test =====\n";
    // this->cfg_file = fmt::format("CFG{}.xml", thread_number);
    pugi::xml_document doc;
    if (!doc.load_file(cfg_file.c_str()))
    {
        std::cerr << fmt::format("run_tests Error loading {}\n", cfg_file);
        return;
    }

    pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");

    // root node for tree
    std::shared_ptr<node> root = std::make_shared<node>();
    // std::time_t now = std::time(NULL);
    // srand(now);
    std::time_t now = std::time(0);
    srand(now);
    int num_nodes = 0;
    bool bad_parse = false;
    Lexer l(this->cfg_file);
    // std::vector<std::string> results;
    for (int i = 0; !bad_parse && i < 40; ++i)
    {
        srand(rand());
        int num_nodes = 0;
        std::shared_ptr<node> root = std::make_shared<node>();
        do
        {
            this->messed_up_word = std::pair<int, std::string>(-1, "NONE");
            root->clear_node();
            root->CLASS = "PROGPRIMEPRIME";
            std::cout << i << ": Generating tree\n";
            num_nodes = this->generate_tree(root, productions, -1, component::SCOPE_CHECK);
            std::cout << i << ": Generated tree\n";
        } while (num_nodes < 300);
        this->scope_errors.clear();
        std::cout << i << ": Tree generation completed\n";
        std::ofstream rand_tree;
        std::ofstream code_file;

        construct_ftables(root, 0, component::SCOPE_CHECK);

        std::cout << i << ": FTables constructed\n";
        this->num_terminals = 0;
        populate_identifiers(root, component::SCOPE_CHECK);
        std::cout << i << ": Identidiers populated\n";
        std::string tree = root->printnode(0, "testScopeChecker()");
        std::cout << i << ": Tree printed ()\n";
        tree = root->printnode(0, "testScopeChecker()");
        std::ofstream tree_file(fmt::format("thread_{}_generated_ast{}.xml", thread_number, i));
        tree_file << tree;
        tree_file.close();
        std::cout << i << ": Parsed thread_{}_generated_ast{}.xml\n";
        code_file.open(fmt::format("thread_{}_code_file{}.txt", thread_number, i));
        std::string plaintext_code = root->print_code(0, code_file);
        std::cout << i << ": code printed ()\n";
        code_file << plaintext_code;
        code_file.close();
        std::cout << i << ": Printed thread_{}_code_file{}.txt\n";
        // // do something with lexer
        l.read_input(fmt::format("./thread_{}_code_file{}.txt", thread_number, i));
        std::cout << i << ": Lexer input read\n";
        bool lex_res = l.lex(true, fmt::format("./thread_{}_token_stream{}.xml", thread_number, i));
        std::cout << i << ": Lexed\n";
        if (!lex_res)
        {
            std::cout << "Lexing failed\n";
            return;
        }
        Parser p(this->cfg_file);
        std::shared_ptr<node> parsed_root = p.parse(fmt::format("./thread_{}_parsed_ast{}.xml", thread_number, i), fmt::format("./thread_{}_token_stream{}.xml", thread_number, i));
        // code_file.open(fmt::format("thread_{}_generated_code_file.txt", thread_number));
        std::cout << i << ": Parsed\n";
        // plaintext_code = parsed_root->print_code(0, code_file);
        // code_file << plaintext_code;
        // code_file.close();
        // root is PROGPRIMEPRIME
        std::shared_ptr<node> generated_root = root->get_child(0)->get_child(0);
        if (compare_nodes(generated_root, parsed_root))
        {
            // std::cout << i << ": Parser succeeded\n";
        }
        else
        {
            std::cout << i << ": Parser failed\n";
            bad_parse = true;
            // catch immediately if it fails
            throw;
        }
        Scope_Checker s(parsed_root);
        std::cout << i << ": Running Scope checker ran\n";
        bool scope_res = s.run_scope_checker(thread_number);
        std::string filename = fmt::format("thread_{}_scoped_tree{}.xml", thread_number, i);
        tree_file.open(filename);
        tree_file << s.root->printnode(0, "Scope_Checker");
        tree_file.close();
        std::cout << i << ": Scope checker ran\n";
        {
            results.push_back(fmt::format("{} Result of Scope Check: {}\n", i, scope_res));
            results.push_back(fmt::format("{}\tIntentional Errors: {}\n", i, this->scope_errors.size()));
            int num_intentional_errors = this->scope_errors.size();
            std::string found_errors = "";
            std::string non_errors = "";
            int num_non_errors = 0;
            int cr_counter = 0;
            for (auto e : s.error)
            {
                bool found = false;
                for (auto it = this->scope_errors.begin(); it != this->scope_errors.end(); ++it)
                {
                    if (it->first == e.first)
                    {
                        this->discovered_scope_errors.push_back(*it);
                        this->scope_errors.erase(it);
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    // found_errors.append(e.first).append(", ");
                    found_errors = fmt::format("{}\033[32m{}\033[0m, ", found_errors, e.first);
                }
                else
                {
                    // non_errors.append(e.first).append(", ");
                    non_errors = fmt::format("{}\033[35m{}\033[0m, ", non_errors, e.first);
                    ++num_non_errors;
                }
            }
            std::string missed_errors = "";
            for (auto it = this->scope_errors.begin(); it != this->scope_errors.end(); ++it)
            {
                // missed_errors.append(it->first).append(", ");
                missed_errors = fmt::format("{}\033[33m{}\033[0m, ", missed_errors, it->first);
            }
            results.push_back(fmt::format("{}\t{}/{} Discovered Errors: {}\n", i, s.error.size() - num_non_errors, num_intentional_errors, found_errors));
            results.push_back(fmt::format("{}\t{} Missed Errors: {}\n", i, this->scope_errors.size(), missed_errors));
            results.push_back(fmt::format("{}\t{} False Positives: {}\n", i, num_non_errors, non_errors));
            if (scope_res)
            {
                if (s.error.size() == 0 && this->scope_errors.size() == 0)
                {
                    results.push_back(fmt::format("{}\t\033[32mPassed\033[0m\n\n", i));
                }
                else if (this->scope_errors.size() > 0)
                {
                    results.push_back(fmt::format("{}\t\033[31mFailed\033[0m\n\n", i));
                }
            }
            else
            {
                if (s.error.size() > 0 && this->scope_errors.size() == 0 && non_errors.length() == 0)
                {
                    results.push_back(fmt::format("{}\t\033[32mPassed\033[0m\n\n", i));
                }
                else if (this->scope_errors.size() > 0 || non_errors.length() == 0)
                {
                    results.push_back(fmt::format("{}\t\033[31mFailed\033[0m\n\n", i));
                }
            }
        }
        root->clear_node();
    }
    // for (auto s : results)
    // {
    //     std::cout << s;
    // }
    // std::cout << fmt::format("\033[3{}mThread {}\033[0m testing completed\n", 2 + thread_number, thread_number);
}

void Tester::test_type_checker()
{
    Type_Checker t;
}

void Tester::test_code_generator()
{
    Code_Generator CG;
}