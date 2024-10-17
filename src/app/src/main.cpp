// #include "include/pugixml/pugixml.hpp"
// #include <pugixml/pugixml.hpp>
#include "pugixml.hpp"
#include "automata.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "scope_checker.hpp"
#include <iostream>
#include <fmt/core.h>
int main()
{
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};

    std::cout << "Testing scope checker" << std::endl;

    // std::srand(std::time(0));
    Scope_Checker SC;
    // SC.testScopeChecker(10);
    for (int i = 0; i < 10; ++i)
    {
        Lexer luthor;
        luthor.read_input(fmt::format("./code_file{}.txt", i));
        std::cout << "INPUT READ\n";
        luthor.lex();
        std::cout << "LEXED\n";
        Parser p;
        p.parse(fmt::format("./code_file{}.txt", i));
        // std::cout << "PARSED\n";
    }

    return 0;
}