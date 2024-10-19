// #include "include/pugixml/pugixml.hpp"
// #include <pugixml/pugixml.hpp>
#include "pugixml.hpp"
#include "automata.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "scope_checker.hpp"
// #include
#include <iostream>
#include <fmt/core.h>
#include "testing.hpp"
#include <thread>
void test_function(int num)
{
    Tester test;
    test.cfg_file = fmt::format("CFG{}.xml", num);
    test.run_tests(num);
}
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
    return 0;
}