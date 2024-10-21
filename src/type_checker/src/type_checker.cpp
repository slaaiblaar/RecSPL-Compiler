#include "type_checker.hpp"
#include <iostream>
#include <regex>
// std::string get_child_value(std::shared_ptr<node> n)
// {
//     if (n->CLASS == "VTYP")
//         return n->get_child(0)->WORD;
//     return "";
// }
std::string get_child_value(std::shared_ptr<node> n)
{
    if (n->num_children() > 0 &&
        (n->CLASS == "VNAME" ||
         n->CLASS == "VTYP" ||
         n->CLASS == "FNAME" ||
         n->CLASS == "CONST"))
    {
        return n->get_child(0)->WORD;
    }
    return "";
}
Type_Checker::Type_Checker()
{
    std::cout << "Type Checker\n";
}
std::string Type_Checker::type_of(std::shared_ptr<node> n)
{
    std::regex text("\"[A-Z][a-z]{0,7}\"");

    std::string num1 = "-?0\\.([0-9])*[1-9]";
    std::string num2 = "-?[1-9]([0-9])*(\\.([0-9])*[1-9])?";
    std::regex num(fmt::format("0|{}|{}", num1, num2));
    if (n->CLASS == "LITERAL")
    {
        if (std::regex_search(n->WORD, text))
        {
            return "text";
        }
        if (std::regex_search(n->WORD, num))
        {
            return "num";
        }
    }
    if (n->CLASS == "VNAME" || n->CLASS == "FNAME")
    {
        return (*n->type_table)[n->get_child(0)->WORD];
    }
    if (n->CLASS == "DECL")
    {
        // DECL => HEADER => ftyp (ftyp is keyword)
        return n->get_child(0)->get_child(0)->WORD;
    }
    return "";
}
bool Type_Checker::check(std::shared_ptr<node> n)
{
    if (n->CLASS == "")
        return prog(n);
    if (n->CLASS == "GLOBVARS")
        return globvars(n);
    if (n->CLASS == "VTYP")
        return vtyp(n);
    if (n->CLASS == "VNAME")
        return vname(n);
    if (n->CLASS == "ALGO")
        return algo(n);
    if (n->CLASS == "INSTRUC")
        return instruc(n);
    if (n->CLASS == "COMMAND")
        return command(n);
    if (n->CLASS == "ATOMIC")
        return atomic(n);
    if (n->CLASS == "CONST")
        return cons(n);
    if (n->CLASS == "ASSIGN")
        return assign(n);
    if (n->CLASS == "BRANCH")
        return branch(n);
    if (n->CLASS == "TERM")
        return term(n);
    if (n->CLASS == "OP")
        return op(n);
    if (n->CLASS == "ARG")
        return arg(n);
    if (n->CLASS == "COND")
        return cond(n);
    if (n->CLASS == "SIMPLE")
        return simple(n);
    if (n->CLASS == "COMPOSIT")
        return composit(n);
    if (n->CLASS == "UNOP")
        return unop(n);
    if (n->CLASS == "BINOP")
        return binop(n);
    if (n->CLASS == "FNAME")
        return fname(n);
    if (n->CLASS == "FUNCTIONS")
        return functions(n);
    if (n->CLASS == "DECL")
        return decl(n);
    if (n->CLASS == "HEADER")
        return header(n);
    if (n->CLASS == "BODY")
        return body(n);
    if (n->CLASS == "PROLOG")
        return prolog(n);
    if (n->CLASS == "EPILOG")
        return epilog(n);
    if (n->CLASS == "LOCVARS")
        return locvars(n);
    if (n->CLASS == "SUBFUNCS")
        return subfuncs(n);
    if (n->CLASS == "KEYWORD")
        return keyword(n);
    if (n->CLASS == "LITERAL")
        return keyword(n);

    std::cout << "UNDEFINED CLASS " << n->CLASS << "\n";
    return false;
}

bool Type_Checker::prog(std::shared_ptr<node> n)
{
    std::cout << "Checking PROG\n";
    bool res = true;
    for (auto c : n->get_children())
    {
        res = res && check(c);
    }
    return res;
}
bool Type_Checker::globvars(std::shared_ptr<node> n)
{
    // name's id and type already bound during scope check
    std::cout << "Checking GLOBVARS\n";
    // std::shared_ptr<node> vtyp = n->get_child(0);
    // std::string type = get_child_value(vtyp);
    // std::shared_ptr<node> vname = n->get_child(1);
    // std::string name = get_child_value(vname);
    // bind name -> id -> type, and get back id
    // std::string id = this->v_table.bind(name, type);
    // replace name with id
    // vname->get_child(0)->WORD = id;
    // third child is comma literal, no type checking needed
    if (n->num_children() > 3)
    {
        return check(n->get_child(3));
    }
    return true;
}
bool Type_Checker::vtyp(std::shared_ptr<node> n)
{
    std::cout << "Checking VTYP\n";
    return true;
}
bool Type_Checker::vname(std::shared_ptr<node> n)
{
    std::cout << "Checking VNAME\n";
    return true;
}
bool Type_Checker::algo(std::shared_ptr<node> n)
{
    std::cout << "Checking ALGO\n";
    if (n->num_children() > 2)
    {
        return check(n->get_child(1));
    }
    return true;
}
bool Type_Checker::instruc(std::shared_ptr<node> n)
{
    std::cout << "Checking INSTRUC\n";
    bool res = true;
    // check COMMAND  child
    res = res && check(n->get_child(0));
    if (n->num_children() > 2)
    {
        // check recursive INSTRUC child
        res = res && check(n->get_child(2));
    }
    return res;
}
bool Type_Checker::command(std::shared_ptr<node> n)
{
    std::cout << "Checking COMMAND\n";
    std::shared_ptr<node> c = n->get_child(0);
    if (c->CLASS == "KEYWORD")
    {
        if (c->WORD == "skip" || c->WORD == "halt")
        {
            return true;
        }
        if (c->WORD == "return")
        {
            return true;
        }
    }
    if (n->CLASS == "print")
    {
        std::string atomic_type = type_of(n->get_child(1));
        if (atomic_type == "num" || atomic_type == "text")
            return true;
        return false;
    }
    if (n->CLASS == "return")
    {
        std::string atomic_type = type_of(n->get_child(1));
        std::string fun_type = type_of(n->start_of_scope);
        if (atomic_type == "num" || atomic_type == "text")
            return true;
        return false;
    }
    bool res = true;
}
bool Type_Checker::atomic(std::shared_ptr<node> n)
{
    std::cout << "Checking ATOMIC\n";
}
bool Type_Checker::cons(std::shared_ptr<node> n)
{
    std::cout << "Checking CONST\n";
}
bool Type_Checker::assign(std::shared_ptr<node> n)
{
    std::cout << "Checking ASSIGN\n";
}
bool Type_Checker::branch(std::shared_ptr<node> n)
{
    std::cout << "Checking BRANCH\n";
}
bool Type_Checker::term(std::shared_ptr<node> n)
{
    std::cout << "Checking TERM\n";
}
bool Type_Checker::op(std::shared_ptr<node> n)
{
    std::cout << "Checking OP\n";
}
bool Type_Checker::arg(std::shared_ptr<node> n)
{
    std::cout << "Checking ARG\n";
}
bool Type_Checker::cond(std::shared_ptr<node> n)
{
    std::cout << "Checking COND\n";
}
bool Type_Checker::simple(std::shared_ptr<node> n)
{
    std::cout << "Checking SIMPLE\n";
}
bool Type_Checker::composit(std::shared_ptr<node> n)
{
    std::cout << "Checking COMPOSITE\n";
}
bool Type_Checker::unop(std::shared_ptr<node> n)
{
    std::cout << "Checking UNOP\n";
}
bool Type_Checker::binop(std::shared_ptr<node> n)
{
    std::cout << "Checking BINOP\n";
}
bool Type_Checker::fname(std::shared_ptr<node> n)
{
    std::cout << "Checking FNAME\n";
    return true;
}
bool Type_Checker::functions(std::shared_ptr<node> n)
{
    std::cout << "Checking FUNCTIONS\n";
}
bool Type_Checker::decl(std::shared_ptr<node> n)
{
    std::cout << "Checking DECL\n";
}
bool Type_Checker::header(std::shared_ptr<node> n)
{
    std::cout << "Checking HEADER\n";
}
bool Type_Checker::body(std::shared_ptr<node> n)
{
    std::cout << "Checking BODY\n";
}
bool Type_Checker::prolog(std::shared_ptr<node> n)
{
    std::cout << "Checking PROLOG\n";
    return true;
}
bool Type_Checker::epilog(std::shared_ptr<node> n)
{
    std::cout << "Checking EPILOG\n";
    return true;
}
bool Type_Checker::locvars(std::shared_ptr<node> n)
{
    std::cout << "Checking LOCVARS\n";
}
bool Type_Checker::subfuncs(std::shared_ptr<node> n)
{
    std::cout << "Checking SUBFUNCS\n";
}
bool Type_Checker::keyword(std::shared_ptr<node> n)
{
    std::cout << "Checking KEYWORD\n";
    return true;
}
bool Type_Checker::literal(std::shared_ptr<node> n)
{
    std::cout << "Checking LITERAL\n";
    return true;
}