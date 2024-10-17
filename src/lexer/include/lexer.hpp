#ifndef lexer_h
#define lexer_h
#include "automata.hpp"
class Lexer
{
public:
    Lexer();
    void set_input(std::string input);
    void read_input(std::string file_path);
    bool lex();
    Automaton dfa;

private:
    std::string input;
    std::vector<Token> token_stream;
    std::string file_name;
};
#endif
