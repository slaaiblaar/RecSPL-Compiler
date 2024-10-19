#include "type_checker.hpp"
#include <iostream>
Type_Checker::Type_Checker()
{
    std::cout << "Type Checker\n";
}
std::string Type_Checker::check_type(std::shared_ptr<node> n)
{
}
// gets type from tables, will check first
symbol_table_entry &Type_Checker::get_type(std::shared_ptr<node> n)
{
    symbol_table_entry foo = {"name", {"t1", "t2", "t3", "t4"}};
    return foo;
}