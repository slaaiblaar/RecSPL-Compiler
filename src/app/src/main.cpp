// #include "include/pugixml/pugixml.hpp"
// #include <pugixml/pugixml.hpp>
#include "pugixml.hpp"
#include "automata.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "scope_checker.hpp"
#include "ast_node.hpp"
// #include
#include <iostream>
#include <fmt/core.h>
#include "testing.hpp"
#include <thread>
#include <fstream>
void test_function(int num)
{
    Tester test;
    test.cfg_file = fmt::format("CFG{}.xml", num);
    test.run_tests(num);
}

void test_code_generation()
{
    // moved to testing.cpp:1411:test_code_generator()
}

int main(int argc, char *argv[])
{
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};
    // std::srand(std::time(0));
    // Scope_Checker SC;
    // // SC.testScopeChecker(10);
    // for (int i = 0; i < 10; ++i)
    // {
    //     Lexer luthor;
    //     luthor.read_input(fmt::format("./code_file{}.txt", i));
    //     std::cout << "INPUT READ\n";
    //     luthor.lex(true);

    //     std::cout << "LEXED\n";
    //     Parser p;
    //     std::cout << "input file " << i << "\n";
    //     p.parse(fmt::format("./code_file{}.txt", i));
    //     // std::cout << "PARSED\n";
    // }
    if (argc == 1)
    {
        std::thread t1(test_function, 1);
        // std::thread t2(test_function, 2);
        // std::thread t3(test_function, 3);
        // std::thread t4(test_function, 4);
        t1.join();
        // t2.join();
        // t3.join();
        // t4.join();
        // int num_threads = 4;
        // std::vector<std::shared_ptr<std::thread>> thread_vector;
        // for (int i = 1; i <= num_threads; ++i)
        // {
        //     pugi::xml_document thread_doc;
        //     if (!thread_doc.load_file(fmt::format("CFG{}.xml", i).c_str()))
        //     {
        //         pugi::xml_document original_doc;
        //         if (!original_doc.load_file("CFG.xml"))
        //         {
        //             std::cout << "Cannot load CFG file\n";
        //         }
        //         original_doc.save_file(fmt::format("CFG{}.xml", i).c_str());
        //     }
        //     thread_vector.push_back(std::make_shared<std::thread>(std::thread(test_function, i)));
        // }

        // for (auto t : thread_vector)
        // {
        //     t->join();
        // }
    }
    else if (argc > 1)
    {
        // Intermediate code generation test:
        std::string input_file(argv[1]);
        std::cout << "Phase A code generation test:" << std::endl;
        test_code_generation();
        Lexer l("CFG.xml");
        // l.read_input("thread_1_code_file0.txt");
        l.read_input(input_file);
        std::ifstream i_file(input_file);
        if (!i_file.good())
        {
            std::cout << input_file << " does not exist\n";
            return 1;
        }
        if (!l.lex(false, "token_stream.xml"))
        {
            std::cout << "Lexing failed\n";
            return 1;
        }
        Parser p("CFG.xml", input_file);
        std::shared_ptr<node> root = p.parse("ast.xml");
        if (root->NAME == "ERROR")
        {
            std::cout << "Parsing failed\n";
            return 1;
        }
        Scope_Checker s(root, "CFG.xml");
        s.run_scope_checker();
        if (s.error.size() > 0)
        {
            std::cout << "Scope checker failed\n";
            for (auto e : s.error)
            {
                std::cout << e.first;
            }
            return 1;
        }
        std::ofstream s_tree("ast_scope.xml");
        s_tree << root->printnode(0, "main");
        Type_Checker t(input_file);
        t.check(root);
        std::cout << root->print_code(0, true);
        if (t.type_errors.size() > 0)
        {
            std::cout << "Type checker failed\n";
            for (auto e : t.type_errors)
            {
                std::cout << e.first;
            }
            return 1;
        }
        else
        {
            std::cout << "Type checker succeeded\n";
        }
    }
    return 0;
}