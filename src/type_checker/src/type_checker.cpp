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
    if (n->CLASS == "ATOMIC")
    {
        // DECL => HEADER => ftyp (ftyp is keyword)
        return type_of(n->get_child(0));
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
    n->type = type_of(n);
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
        if (n->CLASS == "print")
        {
            std::string atomic_type = type_of(n->get_child(1));
            if (atomic_type == "num" || atomic_type == "text")
                return true;
            std::string t_error = fmt::format("Invalid type of print argument: {}", atomic_type);
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n));
        }
        if (n->CLASS == "return")
        {
            std::string atomic_type = type_of(n->get_child(1));
            std::string fun_type = type_of(n->start_of_scope);
            if (fun_type == atomic_type)
                return true;
            std::string t_error = fmt::format("Invalid return type for function of type {}: returns {}", fun_type, atomic_type);
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n));
        }
        return false;
    }
    // ASSIGN and BRANCH
    if (n->num_children() == 1)
    {
        return check(n->get_child(0));
    }
    // must be FNAME ( ATOMIC , ATOMIC , ATOMIC )
    bool res = true;
    res = res && check(n->get_child(0));
    if (type_of(n->get_child(2)) != "num")
    {
        std::string t_error = fmt::format("Invalid function parameter type: ", type_of(n->get_child(2)));
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
        res = false;
    }
    if (type_of(n->get_child(4)) != "num")
    {
        std::string t_error = fmt::format("Invalid function parameter type: ", type_of(n->get_child(4)));
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(4)));
        res = false;
    }
    if (type_of(n->get_child(6)) != "num")
    {
        std::string t_error = fmt::format("Invalid function parameter type: ", type_of(n->get_child(6)));
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(6)));
        res = false;
    }
    return res;
}
bool Type_Checker::atomic(std::shared_ptr<node> n)
{
    std::cout << "Checking ATOMIC\n";
    n->type = type_of(n->get_child(0));
    return true;
}
bool Type_Checker::cons(std::shared_ptr<node> n)
{
    std::cout << "Checking CONST\n";
    n->type = type_of(n->get_child(0));
    return true;
}
bool Type_Checker::assign(std::shared_ptr<node> n)
{
    bool res = true;
    std::cout << "Checking ASSIGN\n";
    if (n->num_children() == 2)
    {
        res = type_of(n->get_child(0)) == "num";
        if (!res)
        {
            std::string t_error = fmt::format("Invalid function parameter type: ", type_of(n->get_child(6)));
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(6)));
        }
    }
    if (n->get_child(2)->CLASS == "TERM")
    {
        return type_of(n->get_child(0)) == type_of(n->get_child(2));
    }
    // bool res = true;
    // res = res && check(n->get_child(0));
    res = res && (type_of(n->get_child(4)) == "num");
    res = res && (type_of(n->get_child(6)) == "num");
    res = res && (type_of(n->get_child(8)) == "num");
    res = res && (type_of(n->get_child(0)) == type_of(n->get_child(2)));
    return res;
}
bool Type_Checker::branch(std::shared_ptr<node> n)
{
    std::cout << "Checking BRANCH\n";
    bool res = true;
    res = res && check(n->get_child(1));
    res = res && n->get_child(1)->type == "bool";
    res = res && check(n->get_child(3));
    res = res && check(n->get_child(5));
    return res;
}
bool Type_Checker::term(std::shared_ptr<node> n)
{
    std::cout << "Checking TERM\n";
    bool res = true;
    res = check(n->get_child(0));
    n->type = n->get_child(0)->type;
    return res;
}
bool Type_Checker::op(std::shared_ptr<node> n)
{
    std::cout << "Checking OP\n";
    bool res = true;
    res = res && check(n->get_child(0));
    res = res && check(n->get_child(2));
    if (n->num_children() > 3)
    {
        res = res && check(n->get_child(4));
        if (n->get_child(0)->type == n->get_child(2)->type &&
            n->get_child(0)->type == n->get_child(4)->type &&
            n->get_child(0)->type == "bool")
        {
            n->type = "bool";
            res = res && true;
            return res;
        }
        res = res && check(n->get_child(4));
        if (n->get_child(0)->type == n->get_child(2)->type &&
            n->get_child(0)->type == n->get_child(4)->type &&
            n->get_child(0)->type == "num")
        {
            n->type = "num";
            res = res && true;
            return res;
        }
        res = res && check(n->get_child(4));
        if (n->get_child(2)->type == "num" &&
            n->get_child(2)->type == n->get_child(4)->type &&
            n->get_child(0)->type == "compare")
        {
            n->type = "bool";
            res = res && true;
            return res;
        }
    }
    else if (n->get_child(0)->type == n->get_child(2)->type)
    {
        n->type = n->get_child(0)->type;
        res = res && true;
    }
    else
    {
        n->type = "undefined";
        res = false;
    }
    return res;
}
bool Type_Checker::unop(std::shared_ptr<node> n)
{
    std::cout << "Checking UNOP\n";
    if (n->get_child(0)->WORD == "not")
    {
        n->type = "bool";
    }
    else
    {
        n->type = "num";
    }
    return true;
}
bool Type_Checker::binop(std::shared_ptr<node> n)
{
    std::cout << "Checking BINOP\n";
    if (n->get_child(0)->WORD == "or" || n->get_child(0)->WORD == "and")
    {
        n->type = "bool";
    }
    else if (n->get_child(0)->WORD == "eq" || n->get_child(0)->WORD == "grt")
    {
        n->type = "compare";
    }
    else
    {
        n->type = "num";
    }
    return true;
}
bool Type_Checker::arg(std::shared_ptr<node> n)
{
    std::cout << "Checking ARG\n";
    check(n->get_child(0));
    n->type = n->get_child(0)->type;
    return true;
}
bool Type_Checker::cond(std::shared_ptr<node> n)
{
    std::cout << "Checking COND\n";
    bool res = check(n->get_child(0));
    n->type = n->get_child(0)->type;
    return res;
}
bool Type_Checker::simple(std::shared_ptr<node> n)
{
    std::cout << "Checking SIMPLE\n";
    bool res = true;
    res = res && check(n->get_child(0));
    res = res && check(n->get_child(2));
    res = res && check(n->get_child(4));
    if (n->get_child(0)->type == n->get_child(2)->type &&
        n->get_child(0)->type == n->get_child(4)->type &&
        n->get_child(0)->type == "bool")
    {
        n->type = "bool";
        res = res && true;
    }
    res = res && check(n->get_child(4));
    if (n->get_child(2)->type == "num" &&
        n->get_child(2)->type == n->get_child(4)->type &&
        n->get_child(0)->type == "compare")
    {
        n->type = "bool";
        res = res && true;
    }
    else
    {
        n->type = "undefined";
        res = false;
    }

    return res;
}
bool Type_Checker::composit(std::shared_ptr<node> n)
{
    std::cout << "Checking COMPOSITE\n";
    bool res = true;
    res = res && check(n->get_child(0));
    res = res && n->get_child(0)->type == "bool";
    res = res && check(n->get_child(2));
    res = res && n->get_child(2)->type == "bool";
    if (n->num_children() > 3)
    {
        res = res && check(n->get_child(4));
        res = res && n->get_child(4)->type == "bool";
    }
    if (!res)
    {
        n->type = "undefined";
    }
    else
    {
        n->type = "bool";
    }
    return res;
}
bool Type_Checker::fname(std::shared_ptr<node> n)
{
    std::cout << "Checking FNAME\n";
    n->type = type_of(n);
    return true;
}
bool Type_Checker::functions(std::shared_ptr<node> n)
{
    std::cout << "Checking FUNCTIONS\n";
    bool res = true;
    res = res && check(n->get_child(0));
    if (n->num_children() > 1)
    {
        res = res && check(n->get_child(1));
    }
    return res;
}
bool Type_Checker::decl(std::shared_ptr<node> n)
{
    std::cout << "Checking DECL\n";
    bool res = true;
    res = res && check(n->get_child(0));
    res = res && check(n->get_child(1));
    return res;
}
bool Type_Checker::header(std::shared_ptr<node> n)
{
    std::cout << "Checking HEADER\n";
    check(n->get_child(1));
    bool res = true;
    check(n->get_child(3));
    check(n->get_child(5));
    check(n->get_child(7));
    res = res && n->get_child(3)->type == "num";
    res = res && n->get_child(5)->type == "num";
    res = res && n->get_child(7)->type == "num";
    return res;
}
bool Type_Checker::body(std::shared_ptr<node> n)
{
    std::cout << "Checking BODY\n";
    bool res = true;
    for (auto c : n->get_children())
    {
        res = res && check(c);
    }
    return res;
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
    return true; // already bound when binding scope
}
bool Type_Checker::subfuncs(std::shared_ptr<node> n)
{
    std::cout << "Checking SUBFUNCS\n";
    return check(n->get_child(0));
}
bool Type_Checker::keyword(std::shared_ptr<node> n)
{
    std::cout << "Checking KEYWORD\n";
    return true;
}
bool Type_Checker::literal(std::shared_ptr<node> n)
{
    std::cout << "Checking LITERAL\n";
    n->type = type_of(n);
    return true;
}