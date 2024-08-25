// #include "include/pugixml/pugixml.hpp"
// #include <pugixml/pugixml.hpp>
#include "pugixml.hpp"
#include "automata.hpp"
#include <iostream>
int main()
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("./CFG.xml");
    if (!result)
    {
        std::cout << "Load result: " << result.description() << std::endl;
        return -1;
    }
    pugi::xml_node lhs = doc.child("CFG").child("PRODUCTIONRULES").first_child();
    do
    {
        std::cout << std::endl
                  << lhs.name() << ":";
        pugi::xml_node production = lhs.first_child();
        do
        {
            std::cout << "\n\trhs:";
            pugi::xml_node rhs = production.first_child();
            do
            {
                if (rhs.attribute("terminal").as_bool() == false)
                {
                    std::cout << " " << rhs.name();
                    continue;
                }
                else
                {
                    std::cout << " " << rhs.first_child().child_value();
                }
                // std::cout << " " << rhs.name() << "(" << rhs.attribute("terminal").value() << ")";
            } while ((rhs = rhs.next_sibling()) != pugi::xml_node());
        } while ((production = production.next_sibling()) != pugi::xml_node());

    } while ((lhs = lhs.next_sibling()) != pugi::xml_node());
    std::cout << std::endl;
    Automaton automaton;
    // std::cout << "\n"
    //           << automaton.append_keyword("a", "TEST") << std::endl;
    std::cout << automaton.append_pattern("\"[A-Z][a-z][a-z0-9]\"", "TEST") << std::endl;
    //-[1-9]([0-9])*.([0-9])*[1-9]
    // std::cout << automaton.append_pattern("-[1-9]([0-9])*.([0-9])*[1-9]", "N") << std::endl;
    // //                                          ((a((b)c))d)
    // std::cout << automaton.append_pattern("((((a(((b))c))d)))", "TEST") << std::endl;
    // std::cout << automaton.append_pattern("\"never\"", "TEST") << std::endl;
    // std::cout << automaton.append_pattern("\"gonna\"", "TEST") << std::endl;
    // std::cout << automaton.append_pattern("\"give\"", "TEST") << std::endl;
    // std::cout << automaton.append_pattern("\"you\"", "TEST") << std::endl;
    // std::cout << automaton.append_pattern("\"up\"", "TEST") << std::endl;
    // automaton.print_nfa();

    pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
    pugi::xml_node keywords_terminal = terminals.child("KEYWORD").first_child();
    pugi::xml_node id_terminal = terminals.child("ID").first_child();
    pugi::xml_node literal_terminal = terminals.child("LITERAL").first_child();
    // do
    // {
    //     // std::cout << keywords_terminal.name() << std::endl;
    //     // std::cout << "Adding " << keywords_terminal.attribute("class").value() << ": " << keywords_terminal.child_value() << std::endl;
    //     automaton.append_keyword(keywords_terminal.child_value(), keywords_terminal.attribute("class").value());
    // } while ((keywords_terminal = keywords_terminal.next_sibling()) != pugi::xml_node());
    // prints NFA state to terminal in a format that's easy to manually insert into render-automata.py
    automaton.print_nfa();
    automaton.nfa_to_dfa();
    automaton.print_dfa();
    return 0;
}