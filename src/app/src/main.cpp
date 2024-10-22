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
void test_function(int num)
{
    Tester test;
    test.cfg_file = fmt::format("CFG{}.xml", num);
    test.run_tests(num);
}

#include "code_generator.hpp"
#include "ast_node.hpp"
#include <iostream>
#include <memory>

void test_code_generation() {
    // Create the AST for the function: num main(num x, num y, num z) { num a; a = x + y; if (a > z) { return a; } else { return z; } }

    // Root node - Function declaration (DECL)
    auto root = std::make_shared<node>();
    root->CLASS = "DECL";

    // Function header: num main(num x, num y, num z)
    auto header = std::make_shared<node>();
    header->CLASS = "HEADER";

    auto return_type = std::make_shared<node>();
    return_type->CLASS = "LITERAL";  // Return type
    return_type->WORD = "num";

    auto function_name = std::make_shared<node>();
    function_name->CLASS = "FID";  // Function name
    function_name->WORD = "main";

    auto param1 = std::make_shared<node>();
    param1->CLASS = "VID";  // Parameter 1
    param1->WORD = "x";

    auto param2 = std::make_shared<node>();
    param2->CLASS = "VID";  // Parameter 2
    param2->WORD = "y";

    auto param3 = std::make_shared<node>();
    param3->CLASS = "VID";  // Parameter 3
    param3->WORD = "z";

    // Attach header children
    header->add_child(return_type, 0);
    header->add_child(function_name, 1);
    header->add_child(param1, 2);
    header->add_child(param2, 3);
    header->add_child(param3, 4);

    // Function body
    auto body = std::make_shared<node>();
    body->CLASS = "BODY";

    // Local variables (LOCVARS): num a;
    auto locvars = std::make_shared<node>();
    locvars->CLASS = "LOCVARS";

    auto var_type = std::make_shared<node>();
    var_type->CLASS = "LITERAL";  // Variable type
    var_type->WORD = "num";

    auto var_name = std::make_shared<node>();
    var_name->CLASS = "VID";  // Variable name
    var_name->WORD = "a";

    locvars->add_child(var_type, 0);
    locvars->add_child(var_name, 1);

    // Assignment: a = x + y;
    auto assign = std::make_shared<node>();
    assign->CLASS = "ASSIGN";

    auto var_a = std::make_shared<node>();
    var_a->CLASS = "VID";
    var_a->WORD = "a";

    auto binop = std::make_shared<node>();
    binop->CLASS = "BINOP";
    binop->WORD = "+";

    auto x_var = std::make_shared<node>();
    x_var->CLASS = "VID";
    x_var->WORD = "x";

    auto y_var = std::make_shared<node>();
    y_var->CLASS = "VID";
    y_var->WORD = "y";

    binop->add_child(x_var, 0);
    binop->add_child(y_var, 1);
    assign->add_child(var_a, 0);
    assign->add_child(binop, 1);

    // If-then-else (BRANCH): if (a > z) { return a; } else { return z; }
    auto branch = std::make_shared<node>();
    branch->CLASS = "BRANCH";

    auto cond = std::make_shared<node>();
    cond->CLASS = "BINOP";
    cond->WORD = ">";

    auto a_var = std::make_shared<node>();
    a_var->CLASS = "VID";
    a_var->WORD = "a";

    auto z_var = std::make_shared<node>();
    z_var->CLASS = "VID";
    z_var->WORD = "z";

    cond->add_child(a_var, 0);
    cond->add_child(z_var, 1);

    // True branch: return a;
    auto return_a = std::make_shared<node>();
    return_a->CLASS = "RETURN";

    auto ret_a = std::make_shared<node>();
    ret_a->CLASS = "VID";
    ret_a->WORD = "a";

    return_a->add_child(ret_a, 0);

    // False branch: return z;
    auto return_z = std::make_shared<node>();
    return_z->CLASS = "RETURN";

    auto ret_z = std::make_shared<node>();
    ret_z->CLASS = "VID";
    ret_z->WORD = "z";

    return_z->add_child(ret_z, 0);

    branch->add_child(cond, 0);
    branch->add_child(return_a, 1);
    branch->add_child(return_z, 2);

    // Attach everything to the body (locvars + assignment + branch)
    body->add_child(locvars, 0);
    body->add_child(assign, 1);
    body->add_child(branch, 2);

    // Attach header and body to root
    root->add_child(header, 0);
    root->add_child(body, 1);

    // Instantiate the Code Generator
    Code_Generator cg;

    // Generate intermediate code
    std::string output = cg.generate(root);

    // Output the generated intermediate code
    std::cout << "Generated Intermediate Code:\n" << output << std::endl;

    // Generate final code
    std::string final_code = cg.generate_final(root);

    // Output the final executable code
    std::cout << "Generated Final Code:\n" << final_code << std::endl;
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

    // Intermediate code generation test:
    std::cout << "Code generator test" << std::endl;
    test_code_generation();

    return 0;
}