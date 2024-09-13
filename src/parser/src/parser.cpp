#include "parser.hpp"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

Parser::Parser()
{
    pugi::xml_parse_result result = this->cfg_doc.load_file("./CFG.xml");
    // really shouldn't happen but w/e, TS habits
    if (!result)
    {
        std::cout << "CFG XML file does not exist\n";
        return;
    }
    this->get_nullable();
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

    // std::cout << "Nullable Values:\n";
    // for (auto const &pair : this->nullable)
    // {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }
}