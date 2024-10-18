
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

bool gen_automaton(std::string terminal_class, std::unordered_map<std::string, std::shared_ptr<Automaton>> &automata, std::string cfg_file = "CFG.xml")
{
    std::shared_ptr<Automaton> term_automaton = std::make_shared<Automaton>();
    pugi::xml_document doc;
    if (!doc.load_file(cfg_file.c_str()))
    {
        std::cerr << fmt::format("gen_automaton Error loading {}\n", cfg_file);
        automata[terminal_class] = nullptr;
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

int Tester::generate_tree(std::shared_ptr<node> parent, pugi::xml_node productions, int depth, component test)
{
    if (parent->subtree_generated)
    {
        return node::node_id_counter;
    }
    parent->subtree_generated = true;
    // for use in testing type checker
    thread_local static int num_number_v = 0;
    thread_local static int num_text_v = 0;
    thread_local static int num_number_f = 0;
    thread_local static int num_void_f = 0;
    thread_local static int node_counter = 0;
    // static int num_terminals = 0;
    if (depth == -1)
    {
        node_counter = 0;
        depth = 0;
        node::node_id_counter = 1;
        num_terminals = 0;
        num_number_v = 0;
        num_text_v = 0;
        num_number_f = 0;
        num_void_f = 0;
    }
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
        bool force_var = (num_number_v < 3 || num_number_v < 3);
        bool force_func = (num_number_f < 3 || num_void_f < 3);
        if (force_var || force_func || depth > 60 || node_counter > 1200)
        {
            // std::cout << std::string(depth, ' ') << parent->CLASS << fmt::format(" point NT point 3 force_var={} force_func={}\n", force_var, force_func);
            if (parent->WORD == "GLOBVARS")
            {
                if (force_var)
                {
                    rhs = rhs_list[1];
                    if (rhs.child("GLOBVARS") == pugi::xml_node())
                    {
                        std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL FUNCTIONS\n";
                    }
                }
                else if (!force_func)
                {
                    rhs = rhs_list[0];
                    if (rhs.child("FUNCTIONS") != pugi::xml_node())
                    {
                        std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL\n";
                    }
                }
            }
            else if (parent->WORD == "FUNCTIONS")
            {
                if (force_func)
                {
                    rhs = rhs_list[1];
                    if (rhs.child("FUNCTIONS") == pugi::xml_node())
                    {
                        std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL FUNCTIONS\n";
                    }
                }
                else if (!force_var)
                {
                    rhs = rhs_list[0];
                    if (rhs.child("FUNCTIONS") != pugi::xml_node())
                    {
                        std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL\n";
                    }
                }
            }
            else if (parent->WORD == "INSTRUC")
            {
                rhs = rhs_list[0];
                if (rhs.child("INSTRUC") != pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of INSTRUC is not INSTRUC ==> COMMAND ;\n";
                }
            }
            else if (parent->WORD == "COMMAND")
            {
                do
                {
                    rhs = rhs_list[rand() % rhs_list.size()];
                } while (rhs.child("BRANCH") != pugi::xml_node());
            }
            else
            {
                rhs = rhs_list[rand() % rhs_list.size()];
            }
        }
        if (rhs == pugi::xml_node())
        {
            // std::cout << std::string(depth, ' ') << parent->CLASS << " point NT point 4\n";
            rhs = rhs_list[rand() % rhs_list.size()];
        }
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
            generate_tree(child, productions, depth + 1, test);

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
            // pugi::xpath_node term_xpath_node = productions.select_node(child->CLASS.c_str());
            if (symbol.attribute("terminal").as_bool()) // if terminal
            {
                // generate tree can't guess the correct keyword
                if (child->CLASS == "KEYWORD")
                {
                    child->WORD = symbol.child("value").child_value();
                    // 0.1 % chance of incorrect keyword
                    bool mess_up = (((double)rand() / (double)RAND_MAX) < 0.001);
                    if (test == component::LEXER && mess_up && this->messed_up_word.first < 0)
                    {
                        child->WORD = fmt::format("random_{}({})", std::to_string(rand() % 5), child->WORD);
                        this->messed_up_word = std::pair<int, std::string>(num_terminals, child->WORD);
                        // std::cout << fmt::format("{}Incorrect token generated: {}: \"{}\"\n", std::string(depth * 2, ' '), num_terminals, child->WORD);
                        // std::cout << child->printnode(depth, "generate_tree()");
                        child->was_printed = false;
                    }
                }
                if (parent->CLASS == "HEADER")
                {
                    if (child->WORD == "num")
                    {
                        ++num_number_f;
                    }
                    if (child->WORD == "void")
                    {
                        ++num_void_f;
                    }
                }
                else if (parent->CLASS == "VTYP")
                {
                    if (child->WORD == "num")
                    {
                        ++num_number_v;
                    }
                    if (child->WORD == "text")
                    {
                        ++num_text_v;
                    }
                }
            }

            symbol = symbol.next_sibling();
        }
    }
    else
    {
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
    return node::node_id_counter;
}

// making a type alias because its super annoying to repeatedly define objects
// of type std::shared_ptr<std::unordered_map<std::string, std::string>>
using ftable_type = std::unordered_map<std::string, std::string[4]>;
using vtable_type = std::unordered_map<std::string, std::string>;
// Drill down through successive chains of FUNCTIONS == > FUNCTIONS productions void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)
void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            t->f_table[it->first][i] = it->second[i];
        }
    }
}
void copy_vtable(std::shared_ptr<vtable_type> f, std::shared_ptr<node> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        t->v_table[it->first] = it->second;
    }
}
void copy_ftable(std::shared_ptr<node> f, std::shared_ptr<node> t)
{
    for (auto it = f->f_table.begin(); it != f->f_table.end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            t->f_table[it->first][i] = it->second[i];
        }
    }
}
void copy_vtable(std::shared_ptr<node> f, std::shared_ptr<node> t)
{
    for (auto it = f->v_table.begin(); it != f->v_table.end(); ++it)
    {
        t->v_table[it->first] = it->second;
    }
}
void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<ftable_type> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            (*t)[it->first][i] = it->second[i];
        }
    }
}
std::shared_ptr<ftable_type> preprocess_ftables(std::shared_ptr<node> n, int depth)
{
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
    // generate name
    do
    {
        random_pattern(fid);
    } while (n->f_table.find(fid->WORD) != n->f_table.end());
    // bind type to name
    n->f_table[fid->WORD][0] = header->get_child(0)->WORD;
    std::shared_ptr<ftable_type> synthesized_table = std::make_shared<ftable_type>();
    (*synthesized_table)[fid->WORD][0] = header->get_child(0)->WORD;

    // check if a FUNCTIONS node is child
    bool prog_or_funcs = (n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->num_children() > 1 && n->get_child(n->num_children() - 1)->CLASS == "FUNCTIONS");
    if (prog_or_funcs && has_functions_child)
    {
        // copy current node's f_table to child
        std::shared_ptr<node> f_child = n->get_child(n->num_children() - 1);
        // copy_ftable(n, c);
        copy_ftable(n, f_child);
        std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
        // Copy child's f_table to current node
        copy_ftable(child_ftable, n);
        copy_ftable(child_ftable, synthesized_table);
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
        std::shared_ptr<ftable_type> p = std::make_shared<ftable_type>(n->f_table);
        std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
        copy_ftable(p, c);

        std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
        // Copy child's f_table
        copy_ftable(child_ftable, p);
        // copy to SUBFUNCS if applicable
        if (n->get_child(f_index)->UID != f_child->UID)
        {
            f_child = n->get_child(f_index);
            std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
            copy_ftable(p, c);
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
        if (id_probability > 0.05)
        {
            // std::cout << fmt::format("  {} existing id\n", n->CLASS);
            int rand_id = 0;
            if (n->CLASS == "VID")
            {
                rand_id = rand() % n->v_table.size();
            }
            else
            {
                rand_id = rand() % n->f_table.size();
            }
            int index = 0;
            auto vit = n->v_table.begin();
            auto fit = n->f_table.begin();
            for (; index < rand_id; ++index)
            {
                if (index == rand_id)
                {
                    if (n->CLASS == "VID")
                    {
                        n->WORD = vit->first;
                    }
                    else
                    {
                        n->WORD = fit->first;
                    }
                    return;
                }
                ++vit;
                ++fit;
            }
        }
        // std::cout << fmt::format("  {} random id\n", n->CLASS);
        // will skip loop and choose random id if none are in scope
        // 5% chance of identifier being random string if none exists
        id_probability = (double)rand() / (double)RAND_MAX;

        if (test == component::LEXER && this->messed_up_word.first < 0 && id_probability < 0.05)
        {

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
        else
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
        return;
    }
    if (n->CLASS == "GLOBVARS")
    {
        std::shared_ptr<node> vtype = n->get_child(0);
        std::shared_ptr<node> vname = n->get_child(1);
        do
        {
            random_pattern(vname->get_child(0));
            // while the name is not unique
        } while (n->v_table.find(vname->get_child(0)->WORD) != n->v_table.end());
        n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
        // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
    }
    // LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
    //              0     1   2   3    4   5  6     7   8
    if (n->CLASS == "LOCVARS")
    {
        for (int i = 0; i < 7; i += 3)
        {
            std::shared_ptr<node> vtype = n->get_child(i);     // 0, 3, 6
            std::shared_ptr<node> vname = n->get_child(i + 1); // 1, 4, 7
            do
            {
                random_pattern(vname->get_child(0));
                // while the name is not unique
            } while (n->v_table.find(vname->get_child(0)->WORD) != n->v_table.end());
            n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
            // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
        }
    }
    for (auto c : n->get_children())
    {
        std::shared_ptr<vtable_type> pt = std::make_shared<vtable_type>(n->v_table);
        std::shared_ptr<vtable_type> ct = std::make_shared<vtable_type>(c->v_table);
        copy_vtable(n, c);
        populate_identifiers(c, test);
        if (c->CLASS == "GLOBVARS" || c->CLASS == "LOCVARS")
        {
            std::shared_ptr<vtable_type> pt = std::make_shared<vtable_type>(n->v_table);
            std::shared_ptr<vtable_type> ct = std::make_shared<vtable_type>(c->v_table);
            copy_vtable(c, n);
            // std::cout << fmt::format("  {}: Copied vtable from child {}\n", n->CLASS, c->CLASS);
        }
    }
}
void Tester::test_scope_checker()
{
}

void Tester::run_tests(int thread_number)
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
    for (int i = 0; !bad_lex && i < 100; ++i)
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
        std::vector<std::string> lex_results;
        if (this->messed_up_word.first > 0 && !lex_res)
        {
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m:   Successfully failed lexing\n\tNumber of tokens lexed:{}, Incorrect token: {}: \"{}\"\n", 2 + thread_number, thread_number, tok_counter, this->messed_up_word.first, this->messed_up_word.second));
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m\tLexer error message: \"{}\"\n\n", 2 + thread_number, thread_number, l.message));
        }
        else if (this->messed_up_word.first <= 0 && !lex_res)
        {
            std::cout << "REALLY SHOULDN'T BE HERE\n";
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m: Unsuccessfully failed when it should have passed\n\tNumber of tokens lexed:{}\n", 2 + thread_number, thread_number, tok_counter));
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m\tLexer error message: \"{}\"\n\n", 2 + thread_number, thread_number, l.message));
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
        {
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m: Unsuccessfully passed when it should have failed\n", 2 + thread_number, thread_number));
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m\tNumber of tokens lexed:{}, Incorrect token: {}: \"{}\"\n\n", 2 + thread_number, thread_number, tok_counter, this->messed_up_word.first, this->messed_up_word.second));
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
            lex_results.push_back(fmt::format("\033[3{}m{}\033[0m:   Successfully passed lexing\n\tNumber of tokens lexed:{}, number of generated terminals: {}\n\n", 2 + thread_number, thread_number, tok_counter, this->num_terminals));
        }
        for (auto lr : lex_results)
        {
            std::cout << lr;
        }
    }
    std::cout << fmt::format("\033[3{}mThread {}\033[0m testing completed\n", 2 + thread_number, thread_number);
}