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
    std::cout << "\nBefore nfa tests\n";
    Automaton automata(AutomatonClass::NFA);
    std::cout << "\n"
              << automata.string_to_automaton("a", "TEST") << std::endl;
    std::cout << automata.pattern_to_automaton("\"[A-Z][a-z][a-z0-9]\"", "TEST") << std::endl;
    //-[1-9]([0-9])*.([0-9])*[1-9]
    std::cout << automata.pattern_to_automaton("-[1-9]([0-9])*.([0-9])*[1-9]", "TEST") << std::endl;
    //                                          ((a((b)c))d)
    std::cout << automata.pattern_to_automaton("((((a(((b))c))d)))", "TEST") << std::endl;
    return 0;
}