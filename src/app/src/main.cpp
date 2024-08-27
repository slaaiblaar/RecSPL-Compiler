// #include "include/pugixml/pugixml.hpp"
// #include <pugixml/pugixml.hpp>
#include "pugixml.hpp"
#include "automata.hpp"
#include "lexer.hpp"
#include <iostream>
int main()
{
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};

    Lexer luthor;
    luthor.read_input("./imput.txt");
    luthor.read_input("./input.txt");
    luthor.lex();
    return 0;
}