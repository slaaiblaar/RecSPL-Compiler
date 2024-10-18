#ifndef lexer_h
#define lexer_h
#include "automata.hpp"
class Lexer
{
public:
    Lexer(std::string cfg_file);
    void set_input(std::string input);
    void read_input(std::string file_path);
    bool lex(bool testing, std::string dest_file = "token_stream.xml");
    Automaton dfa;
    std::vector<Token> tokens;
    void print_tokens(std::string fname = "token_stream.xml");
    std::string message = "";
    std::string cfg_file = "CFG.xml";

private:
    std::string input;
    std::vector<Token> token_stream;
    std::string file_name;
};
#endif
