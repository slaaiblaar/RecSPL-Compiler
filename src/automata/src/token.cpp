#include "automata.hpp"
#include <string>

thread_local int Token::token_counter = 1;
Token::Token(std::string word, std::string token_class)
    : token_word(word), token_class(token_class)
{
    this->token_id = token_counter++;
}