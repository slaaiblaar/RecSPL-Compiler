#include "type_checker.hpp"
#include <iostream>

std::string get_type(std::shared_ptr<node> n)
{
    if (n->CLASS == "VTYP")
        return n->get_child(0)->WORD;
    return "";
}
std::string get_name(std::shared_ptr<node> n)
{
    if (n->CLASS == "VNAME")
        return n->get_child(0)->WORD;
    return "";
}
long symbol_table::num_counter = 0;
int symbol_table::string_counter = 65;
Type_Checker::Type_Checker()
{
    std::cout << "Type Checker\n";
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
    bool res = true;
    for (auto c : n->get_children())
    {
        res = res && check(c);
    }
    return res;
}
bool Type_Checker::globvars(std::shared_ptr<node> n)
{
    std::shared_ptr<node> vtyp = n->get_child(0);
    std::string type = get_type(vtyp);
    std::shared_ptr<node> vname = n->get_child(1);
    std::string name = get_name(vname);
    // bind name -> id -> type, and get back id
    std::string id = this->v_table.bind(name, type);
    // replace name with id
    vname->get_child(0)->WORD = id;
    // third child is comma literal, no type checking needed
    if (n->num_children() > 3)
    {
        return check(n->get_child(3));
    }
    return true;
}
bool Type_Checker::vtyp(std::shared_ptr<node> n)
{
    return true;
}
bool Type_Checker::vname(std::shared_ptr<node> n)
{
    return true;
}
bool Type_Checker::algo(std::shared_ptr<node> n)
{
    if (n->num_children() > 2)
    {
        return check(n->get_child(1));
    }
    return true;
}
bool Type_Checker::instruc(std::shared_ptr<node> n)
{
    bool res = true;
    res = res && check(n->get_child(0));
    if (n->num_children() > 2)
    {
        res = res && check(n->get_child(2));
    }
    return res;
}
bool Type_Checker::command(std::shared_ptr<node> n)
{
    bool res = true;
}
bool Type_Checker::atomic(std::shared_ptr<node> n)
{
}
bool Type_Checker::cons(std::shared_ptr<node> n)
{
}
bool Type_Checker::assign(std::shared_ptr<node> n)
{
}
bool Type_Checker::branch(std::shared_ptr<node> n)
{
}
bool Type_Checker::term(std::shared_ptr<node> n)
{
}
bool Type_Checker::op(std::shared_ptr<node> n)
{
}
bool Type_Checker::arg(std::shared_ptr<node> n)
{
}
bool Type_Checker::cond(std::shared_ptr<node> n)
{
}
bool Type_Checker::simple(std::shared_ptr<node> n)
{
}
bool Type_Checker::composit(std::shared_ptr<node> n)
{
}
bool Type_Checker::unop(std::shared_ptr<node> n)
{
}
bool Type_Checker::binop(std::shared_ptr<node> n)
{
}
bool Type_Checker::fname(std::shared_ptr<node> n)
{
    return true;
}
bool Type_Checker::functions(std::shared_ptr<node> n)
{
}
bool Type_Checker::decl(std::shared_ptr<node> n)
{
}
bool Type_Checker::header(std::shared_ptr<node> n)
{
}
bool Type_Checker::body(std::shared_ptr<node> n)
{
}
bool Type_Checker::prolog(std::shared_ptr<node> n)
{
    return true;
}
bool Type_Checker::epilog(std::shared_ptr<node> n)
{
    return true;
}
bool Type_Checker::locvars(std::shared_ptr<node> n)
{
}
bool Type_Checker::subfuncs(std::shared_ptr<node> n)
{
}
bool Type_Checker::keyword(std::shared_ptr<node> n)
{
    return true;
}
bool Type_Checker::literal(std::shared_ptr<node> n)
{
    return true;
}