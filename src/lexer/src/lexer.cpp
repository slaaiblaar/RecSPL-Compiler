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

    pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
    pugi::xml_node keywords_terminal = terminals.child("KEYWORD").first_child();
    pugi::xml_node vid_terminal = terminals.child("VID").first_child();
    pugi::xml_node fid_terminal = terminals.child("FID").first_child();
    pugi::xml_node literal_terminal = terminals.child("LITERAL").first_child();
    do
    {
        dfa.append_pattern(vid_terminal.child_value(), vid_terminal.attribute("class").value());
    } while ((vid_terminal = vid_terminal.next_sibling()) != pugi::xml_node());
    do
    {
        dfa.append_pattern(fid_terminal.child_value(), fid_terminal.attribute("class").value());
    } while ((fid_terminal = fid_terminal.next_sibling()) != pugi::xml_node());
    do
    {
        dfa.append_pattern(literal_terminal.child_value(), literal_terminal.attribute("class").value());
    } while ((literal_terminal = literal_terminal.next_sibling()) != pugi::xml_node());
    do
    {
        dfa.append_keyword(keywords_terminal.child_value(), keywords_terminal.attribute("class").value());
    } while ((keywords_terminal = keywords_terminal.next_sibling()) != pugi::xml_node());
    // prints NFA state to terminal in a format that's easy to manually insert into render-automata.py
    // dfa.print_nfa();
    dfa.nfa_to_dfa();
    // dfa.print_dfa();
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};

    for (std::string test_string : dfa_test_strings)
    {
        dfa.set_input(test_string);
        if (!dfa.run())
        {
            std::cout << "Test string failed: " << test_string << std::endl;
            std::cout << "Final State: " << dfa.current_state->id << std::endl;
        }
        else
        {
            std::cout << "Test string passed: " << test_string << std::endl;
            std::cout << "Token class: " << dfa.current_state->token_class << std::endl;
        }
        dfa.reset_dfa();
    }
}

void Lexer::set_input(std::string input)
{
    std::regex whitespaces("\\s+");
    this->input = std::regex_replace(input, whitespaces, " ");
}

void Lexer::read_input(std::string file_path)
{
    if (file_path.length() == 0)
    {
        std::cout << "Empty file path\n";
        return;
    }
    std::ifstream input_file;
    input_file.open(file_path);

    if (input_file.fail())
    {
        std::cout << "File \"" << file_path << "\" does not exist\n";
        return;
    }
    std::stringstream input_stream;
    std::string str;
    while (getline(input_file, str))
    {
        input_stream << str << " ";
    }
    input_file.close();
    str = input_stream.str();
    this->input = str;
    std::cout << "\"" << this->input << "\"" << std::endl;
}

void Lexer::lex()
{
    dfa.print_nfa();
    dfa.print_dfa();
    std::vector<Token> tokens;
    if (input.length() == 0)
    {
        std::cout << "Input not set";
        return;
    }
    dfa.set_input(this->input);
    std::cout << "INPUT SET\n";
    while (dfa.read_pos < this->input.length())
    {
        dfa.reset_dfa();
        bool success = false;
        do
        {
            success = dfa.run();
            std::cout << "DFA RAN\n";
        } while (success && !(dfa.read_pos < input.length() - 1 && isspace(input[dfa.read_pos + 1]) != 0));
        if (dfa.read_pos == input.length())
        {
            break;
        }
        if (dfa.accept_pos == -1)
        {
            std::cout << "invalid syntax at position " << dfa.read_start << std::endl;
            std::cout << "read_start: " << dfa.read_start << ", read_pos: " << dfa.read_pos << ", accept_pos: " << dfa.accept_pos << std::endl;
            break;
        }
        tokens.push_back(dfa.get_token());
    }

    pugi::xml_document token_doc;
    pugi::xml_node token_stream = token_doc.append_child("TOKENSTREAM");
    for (auto t : tokens)
    {
        pugi::xml_node tok = token_stream.append_child("TOK");
        pugi::xml_node id = tok.append_child("ID");
        id.append_child(pugi::node_pcdata)
            .set_value(std::to_string(t.token_id).c_str());

        pugi::xml_node tok_class = tok.append_child("CLASS");
        tok_class.append_child(pugi::node_pcdata)
            .set_value(t.token_class.c_str());

        pugi::xml_node word = tok.append_child("WORD");
        word.append_child(pugi::node_pcdata)
            .set_value(t.token_word.c_str());
    }
    pugi::xml_node tok = token_stream.append_child("TOK");
    pugi::xml_node id = tok.append_child("ID");
    id.append_child(pugi::node_pcdata)
        .set_value(std::to_string(-1).c_str());

    pugi::xml_node tok_class = tok.append_child("CLASS");
    tok_class.append_child(pugi::node_pcdata)
        .set_value("KEYWORD");

    pugi::xml_node word = tok.append_child("WORD");
    word.append_child(pugi::node_pcdata)
        .set_value("$");

    std::cout << "Saving tokens to \"token_stream.xml\": " << token_doc.save_file("./token_stream.xml") << std::endl;
}