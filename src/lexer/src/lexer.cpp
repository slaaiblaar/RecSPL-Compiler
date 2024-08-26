#include "lexer.hpp"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
Lexer::Lexer()
{

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("./CFG.xml");
    if (!result)
    {
        std::cout << "Load result: " << result.description() << std::endl;
        return;
    }
    // pugi::xml_node lhs = doc.child("CFG").child("PRODUCTIONRULES").first_child();
    // do
    // {
    //     std::cout << std::endl
    //               << lhs.name() << ":";
    //     pugi::xml_node production = lhs.first_child();
    //     do
    //     {
    //         std::cout << "\n\trhs:";
    //         pugi::xml_node rhs = production.first_child();
    //         do
    //         {
    //             if (rhs.attribute("terminal").as_bool() == false)
    //             {
    //                 std::cout << " " << rhs.name();
    //                 continue;
    //             }
    //             else
    //             {
    //                 std::cout << " " << rhs.first_child().child_value();
    //             }
    //             // std::cout << " " << rhs.name() << "(" << rhs.attribute("terminal").value() << ")";
    //         } while ((rhs = rhs.next_sibling()) != pugi::xml_node());
    //     } while ((production = production.next_sibling()) != pugi::xml_node());

    // } while ((lhs = lhs.next_sibling()) != pugi::xml_node());
    // std::cout << std::endl;

    pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
    pugi::xml_node keywords_terminal = terminals.child("KEYWORD").first_child();
    pugi::xml_node id_terminal = terminals.child("ID").first_child();
    pugi::xml_node literal_terminal = terminals.child("LITERAL").first_child();
    do
    {
        // std::cout << keywords_terminal.name() << std::endl;
        // std::cout << "Adding " << keywords_terminal.attribute("class").value() << ": " << keywords_terminal.child_value() << std::endl;
        dfa.append_pattern(id_terminal.child_value(), id_terminal.attribute("class").value());
    } while ((id_terminal = id_terminal.next_sibling()) != pugi::xml_node());
    do
    {
        // std::cout << keywords_terminal.name() << std::endl;
        // std::cout << "Adding " << keywords_terminal.attribute("class").value() << ": " << keywords_terminal.child_value() << std::endl;
        dfa.append_pattern(literal_terminal.child_value(), literal_terminal.attribute("class").value());
    } while ((literal_terminal = literal_terminal.next_sibling()) != pugi::xml_node());
    do
    {
        // std::cout << keywords_terminal.name() << std::endl;
        // std::cout << "Adding " << keywords_terminal.attribute("class").value() << ": " << keywords_terminal.child_value() << std::endl;
        dfa.append_keyword(keywords_terminal.child_value(), keywords_terminal.attribute("class").value());
    } while ((keywords_terminal = keywords_terminal.next_sibling()) != pugi::xml_node());
    // prints NFA state to terminal in a format that's easy to manually insert into render-automata.py
    dfa.nfa_to_dfa();
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};

    for (std::string test_string : dfa_test_strings)
    {
        if (!dfa.run(test_string))
        {
            std::cout << "Test string failed: " << test_string << std::endl;
            std::cout << "Final State: " << dfa.current_state->id << std::endl;
        }
        else
        {
            std::cout << "Test string passed: " << test_string << std::endl;
            std::cout << "Token class: " << dfa.current_state->token_class << std::endl;
        }
        dfa.reset();
    }
}

void Lexer::set_input(std::string imput)
{
    std::regex whitespaces("\\s+");
    this->input = std::regex_replace(input, whitespaces, " ");
}

void Lexer::read_input(std::string file_path)
{
    if (file_path.length() == 0)
    {
        return;
    }
    std::ifstream input_file;
    input_file.open(file_path);
    std::stringstream input_stream;
    std::string str;
    while (getline(input_file, str))
    {
        input_stream << str << " ";
    }
    str = input_stream.str();
    std::regex whitespaces("\\s+");
    std::cout << std::regex_replace(input_stream.str(), whitespaces, " ") << std::endl;
}