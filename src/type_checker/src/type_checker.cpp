#include "type_checker.hpp"
#include <iostream>
#include <regex>
// std::string get_child_value(std::shared_ptr<node> n)
// {
//     if (n->CLASS == "VTYP")
//         return n->get_child(0)->WORD;
//     return "";
// }
int depth = 0;
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
Type_Checker::Type_Checker(std::string input_file) : input_file(input_file)
{
    std::cout << "Type Checker\n";
}
std::string Type_Checker::type_of(std::shared_ptr<node> n)
{
    std::cout << fmt::format("{}type_of({})\n", std::string(depth * 2, ' '), n->WORD);
    thread_local static Automaton literal_dfa;
    if (literal_dfa.dfa_states.size() == 0)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(cfg_file.c_str());
        if (!result)
        {
            std::cout << "\nFailed to load CFG file\n";
            throw;
        }

        pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
        pugi::xml_node literal_terminal = terminals.child("LITERAL").first_child();
        do
        {
            literal_dfa.append_pattern(literal_terminal.child_value(), literal_terminal.attribute("class").value());
        } while ((literal_terminal = literal_terminal.next_sibling()) != pugi::xml_node());
        literal_dfa.nfa_to_dfa();
        literal_dfa.print_dfa();
    }
    if (n->CLASS == "LITERAL")
    {
        // std::cout << fmt::format("{}LITERAL, num regex:{} \n", std::string(depth * 2, ' '), num_str);
        literal_dfa.set_input(n->WORD);
        bool success;
        do
        {
            success = literal_dfa.run();
        } while (success && !(literal_dfa.read_pos < n->WORD.length() - 1 && isspace(n->WORD[literal_dfa.read_pos + 1]) != 0));
        if (literal_dfa.accept_pos != -1)
        {
            // std::cout << fmt::format("{}LITERAL, num regex:{} \n", std::string(depth * 2, ' '), num_str);
            Token lit_tok = literal_dfa.get_token();
            if (lit_tok.token_word == n->WORD && lit_tok.token_class == "T_Token")
            {
                n->type = "text";
            }
            else if (lit_tok.token_word == n->WORD && lit_tok.token_class == "N_Token")
            {
                n->type = "num";
            }
            // std::cout << fmt::format("{}Num dfa did not match. start {}, read {}, accept {}\n", std::string(depth * 2, ' '), literal_dfa.read_start, literal_dfa.read_pos, literal_dfa.accept_pos);
        }
        if (n->type == "")
        {
            std::cout << fmt::format("{}No type found for {}\n", std::string(depth * 2, ' '), n->WORD);
        }
        // std::cout << fmt::format("{}New type of {}: {}\n", std::string(depth * 2, ' '), n->WORD, n->type);
        // std::cout << "New type of " << n->WORD << ": " << n->type << "\n";
        return n->type;
    }
    if (n->CLASS == "VNAME" || n->CLASS == "FNAME")
    {
        n->row = n->get_child(0)->row;
        n->col = n->get_child(0)->col;
        n->get_child(0)->type = (*n->type_table)[n->get_child(0)->WORD];

        n->type = n->get_child(0)->type;
        // std::cout << "New type of " << n->WORD << ": " << n->type << "\n";
        // std::cout << "    New type of " << n->get_child(0)->WORD << ": " << n->get_child(0)->type << "\n";
        // std::cout << fmt::format("{}New type of {}: {}\n", std::string(depth * 2, ' '), n->WORD, n->type);
        // std::cout << fmt::format("{}  New type of {}: {}\n", std::string(depth * 2, ' '), n->get_child(0)->WORD, n->get_child(0)->type);

        return n->type;
    }
    if (n->CLASS == "DECL")
    {
        // DECL => HEADER => ftyp (ftyp is keyword)
        n->row = n->get_child(0)->get_child(0)->row;
        n->col = n->get_child(0)->get_child(0)->col;
        n->get_child(0)->get_child(1)->type = n->get_child(0)->get_child(0)->WORD;

        n->get_child(0)->type = n->get_child(0)->get_child(0)->WORD;

        n->type = n->get_child(0)->get_child(0)->WORD;
        // std::cout << "New type of " << n->WORD << ": " << n->type << "\n";
        // std::cout << "    New type of " << n->get_child(0)->WORD << ": " << n->get_child(0)->type << "\n";
        // std::cout << "        New type of " << n->get_child(0)->get_child(1) << ": " << n->get_child(0)->get_child(1)->type << "\n";
        // std::cout << fmt::format("{}New type of {}: {}\n", std::string(depth * 2, ' '), n->WORD, n->type);
        // std::cout << fmt::format("{}  New type of {}: {}\n", std::string(depth * 2, ' '), n->get_child(0)->WORD, n->get_child(0)->type);
        // std::cout << fmt::format("{}    New type of {}: {}\n", std::string(depth * 2, ' '), n->get_child(0)->get_child(1)->WORD, n->get_child(0)->get_child(1)->type);

        return n->get_child(0)->get_child(0)->WORD;
    }
    if (n->CLASS == "ATOMIC" || n->CLASS == "CONST")
    {
        // DECL => HEADER => ftyp (ftyp is keyword)
        n->type = type_of(n->get_child(0));
        n->row = n->get_child(0)->row;
        n->col = n->get_child(0)->col;
        // std::cout << "New type of " << n->WORD << ": " << n->type << "\n";
        // std::cout << fmt::format("{}New type of {}: {}\n", std::string(depth * 2, ' '), n->WORD, n->type);
        return n->type;
    }
    // std::cout << "type_of(" << n->WORD << ") is undefined\n";
    // std::cout << fmt::format("{}Type of {} is undefined\n", std::string(depth * 2, ' '), n->WORD);
    return "";
}
bool Type_Checker::check(std::shared_ptr<node> n)
{
    // int depth = -1;
    if (n->CLASS == "ATOMIC" || n->CLASS == "CONST" || n->CLASS == "LITERAL")
        std::cout << fmt::format("{}Checking type of {}:{}\n", std::string(depth * 2, ' '), n->CLASS, n->UID);
    ++depth;
    bool res = false;
    if (n->CLASS == "PROG")
    {
        res = prog(n);
    }
    else if (n->CLASS == "GLOBVARS")
    {
        res = globvars(n);
    }
    else if (n->CLASS == "VTYP")
    {
        res = vtyp(n);
    }
    else if (n->CLASS == "VNAME")
    {
        res = vname(n);
    }
    else if (n->CLASS == "ALGO")
    {
        res = algo(n);
    }
    else if (n->CLASS == "INSTRUC")
    {
        res = instruc(n);
    }
    else if (n->CLASS == "COMMAND")
    {
        res = command(n);
    }
    else if (n->CLASS == "ATOMIC")
    {
        res = atomic(n);
    }
    else if (n->CLASS == "CONST")
    {
        res = cons(n);
    }
    else if (n->CLASS == "ASSIGN")
    {
        res = assign(n);
    }
    else if (n->CLASS == "BRANCH")
    {
        res = branch(n);
    }
    else if (n->CLASS == "TERM")
    {
        res = term(n);
    }
    else if (n->CLASS == "OP")
    {
        res = op(n);
    }
    else if (n->CLASS == "ARG")
    {
        res = arg(n);
    }
    else if (n->CLASS == "COND")
    {
        res = cond(n);
    }
    else if (n->CLASS == "SIMPLE")
    {
        res = simple(n);
    }
    else if (n->CLASS == "COMPOSIT")
    {
        res = composit(n);
    }
    else if (n->CLASS == "UNOP")
    {
        res = unop(n);
    }
    else if (n->CLASS == "BINOP")
    {
        res = binop(n);
    }
    else if (n->CLASS == "FNAME")
    {
        res = fname(n);
    }
    else if (n->CLASS == "FUNCTIONS")
    {
        res = functions(n);
    }
    else if (n->CLASS == "DECL")
    {
        res = decl(n);
    }
    else if (n->CLASS == "HEADER")
    {
        res = header(n);
    }
    else if (n->CLASS == "BODY")
    {
        res = body(n);
    }
    else if (n->CLASS == "PROLOG")
    {
        res = prolog(n);
    }
    else if (n->CLASS == "EPILOG")
    {
        res = epilog(n);
    }
    else if (n->CLASS == "LOCVARS")
    {
        res = locvars(n);
    }
    else if (n->CLASS == "SUBFUNCS")
    {
        res = subfuncs(n);
    }
    else if (n->CLASS == "KEYWORD")
    {
        res = keyword(n);
    }
    else if (n->CLASS == "LITERAL")
    {
        res = literal(n);
    }
    else
    {
        std::cout << fmt::format("{}UNDEFINED CLASS {}\n", std::string(depth * 2, ' '), n->WORD);
    }
    --depth;
    if (n->CLASS == "ATOMIC" || n->CLASS == "CONST" || n->CLASS == "LITERAL")
        std::cout << fmt::format("{}Completed checking type of {}, result: {}\n", std::string(depth * 2, ' '), n->CLASS, (res ? "PASS" : "FAIL"));
    return res;
}

bool Type_Checker::prog(std::shared_ptr<node> n)
{
    // std::cout << "Checking PROG\n";
    bool res = true;
    std::vector<std::shared_ptr<node>> children = n->get_children();
    std::cout << std::string(depth * 2, ' ');
    for (auto c : children)
    {
        std::cout << c->WORD << " ";
    }
    std::cout << "\n";
    for (auto c : children)
    {

        // std::cout << std::string(depth * 2, ' ') << "\033[33mChecking child " << c->WORD << "(" << c->CLASS << ")\033[0m\n";
        res = check(c) && res;
        // std::cout << std::string(depth * 2, ' ') << "\033[33mChecked child " << c->WORD << "(" << c->CLASS << "), result: " << (res ? "PASS" : "FAIL") << "\033[0m\n";
    }
    return res;
}
bool Type_Checker::globvars(std::shared_ptr<node> n)
{
    // name's id and type already bound during scope check
    // std::cout << "Checking GLOBVARS\n";
    bool res = check(n->get_child(1));
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
        return res && check(n->get_child(3));
    }
    return true;
}
bool Type_Checker::vtyp(std::shared_ptr<node> n)
{
    // std::cout << "Checking VTYP\n";
    return true;
}
bool Type_Checker::vname(std::shared_ptr<node> n)
{
    // std::cout << "Checking VNAME\n";
    n->type = type_of(n);
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    return true;
}
bool Type_Checker::algo(std::shared_ptr<node> n)
{
    // std::cout << "Checking ALGO\n";
    if (n->num_children() > 2)
    {
        return check(n->get_child(1));
    }
    return true;
}
bool Type_Checker::instruc(std::shared_ptr<node> n)
{
    // std::cout << "Checking INSTRUC\n";
    bool res = true;
    // check COMMAND  child
    res = check(n->get_child(0)) && res;
    if (n->num_children() > 2)
    {
        // check recursive INSTRUC child
        res = check(n->get_child(2)) && res;
    }
    return res;
}
bool Type_Checker::command(std::shared_ptr<node> n)
{
    // std::cout << "Checking COMMAND\n";
    std::shared_ptr<node> c = n->get_child(0);
    if (c->CLASS == "KEYWORD")
    {
        if (c->WORD == "skip" || c->WORD == "halt")
        {
            std::cout << fmt::format("{} command type {} returns true\n", std::string(depth * 2, ' '), c->WORD);
            return true;
        }
        if (c->WORD == "print")
        {
            std::shared_ptr<node> atomic = n->get_child(1);
            check(atomic);
            std::string atomic_type = type_of(atomic);
            if (atomic->type == "num" || atomic->type == "text")
            {
                std::cout << fmt::format("{} printing type {} returns true\n", std::string(depth * 2, ' '), atomic_type);
                return true;
            }
            std::string t_error = fmt::format("{}:{}:{} Invalid type of print argument: \"{}\"\n", input_file, atomic->row, atomic->col, atomic->type);
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n));
        }
        if (c->WORD == "return")
        {
            std::shared_ptr<node> atomic = n->get_child(1);
            check(atomic);
            std::string atomic_type = type_of(atomic);
            std::string fun_type = type_of(n->start_of_scope);
            if (fun_type == atomic_type)
            {
                std::cout << fmt::format("{}Valid return type\n", std::string(depth * 2, ' '), c->WORD);
                return true;
            }
            if (n->start_of_scope->CLASS == "PROG")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid \"return\" outside of subroutine\n", input_file, c->row, c->col);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n));
            }
            std::string t_error = fmt::format("{}:{}:{} Invalid return type for function of type {}: {}\n", input_file, atomic->row, atomic->col, type_of(n->start_of_scope), atomic_type);
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n));
        }
        return false;
    }
    // ASSIGN and BRANCH
    else if (n->num_children() == 1)
    {
        return check(n->get_child(0));
    }
    else
    {
        // must be FNAME ( ATOMIC , ATOMIC , ATOMIC )
        bool res = true;
        res = check(n->get_child(0)) && res;
        std::shared_ptr<node> param = n->get_child(2);
        check(param);
        if (type_of(param) != "num")
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
            res = false;
        }
        param = n->get_child(4);
        check(param);
        if (type_of(param) != "num")
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
            res = false;
        }
        param = n->get_child(6);
        check(param);
        if (type_of(param) != "num")
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
            std::cout << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
            res = false;
        }
        return res;
    }
    std::cout << "UNKNOWN COMMAND PRODUCTIONS\n";
}
bool Type_Checker::atomic(std::shared_ptr<node> n)
{
    // std::cout << "Checking ATOMIC\n";
    std::cout << std::string(depth * 2, ' ') << "Checking type of child " << n->get_child(0)->WORD << " \n";
    check(n->get_child(0));
    n->type = n->get_child(0)->type;
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    return true;
}
bool Type_Checker::cons(std::shared_ptr<node> n)
{
    // std::cout << "Checking CONST\n";
    check(n->get_child(0));
    n->type = n->get_child(0)->type;
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    return true;
}
bool Type_Checker::assign(std::shared_ptr<node> n)
{
    bool res = true;
    // std::cout << "Checking ASSIGN\n";
    std::shared_ptr<node> var = n->get_child(0);
    if (n->num_children() == 2)
    {
        check(var);
        res = type_of(var) == "num";
        if (!res)
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid variable type for numeric input: {}\n", input_file, var->row, var->col, type_of(var));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, var));
        }
    }
    else if (n->get_child(2)->CLASS == "TERM")
    {
        res = check(n->get_child(2));
        check(n->get_child(2));
        if (type_of(var) != n->get_child(2)->type)
        {
            res = false;
            std::string t_error = fmt::format("{}:{}:{} Invalid assignment of {} to {} type variable\n", input_file, var->row, var->col, n->get_child(2)->type, type_of(var));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, var));
        }
        return res;
    }
    else if (n->get_child(2)->CLASS == "FNAME")
    {
        std::shared_ptr<node> func = n->get_child(2);
        check(func);
        if (type_of(var) != type_of(func))
        {
            res = false;
            std::string t_error = fmt::format("{}:{}:{} Invalid assignment of {} to {} type variable\n", input_file, var->row, var->col, n->get_child(2)->type, type_of(var));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, func));
        }
        std::shared_ptr<node> param = n->get_child(4);
        check(param);
        if (type_of(param) != "num")
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
            res = false;
        }
        param = n->get_child(6);
        check(param);
        if (type_of(param) != "num")
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
            res = false;
        }
        param = n->get_child(8);
        check(param);
        if (type_of(param) != "num")
        {
            std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
            std::cout << std::string(depth * 2, ' ') << t_error;
            this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
            res = false;
        }
    }
    return res;
}
bool Type_Checker::branch(std::shared_ptr<node> n)
{
    // std::cout << "Checking BRANCH\n";
    bool res = true;
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    res = check(n->get_child(1)) && res;

    std::shared_ptr<node> cond = n->get_child(1);
    res = check(cond);
    if (cond->type != "bool")
    {
        std::string t_error = fmt::format("{}:{}:{} Branch condition must be boolean. Given type: {}\n", input_file, cond->row, cond->col, cond->type);
        std::cout << std::string(depth * 2, ' ') << t_error;
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, cond));
    }
    res = check(n->get_child(3)) && res;
    res = check(n->get_child(5)) && res;
    return res;
}
bool Type_Checker::term(std::shared_ptr<node> n)
{
    // std::cout << "Checking TERM\n";
    bool res = true;
    res = check(n->get_child(0));
    n->type = n->get_child(0)->type;
    return res;
}
bool Type_Checker::op(std::shared_ptr<node> n)
{
    // std::cout << "Checking OP\n";
    bool res = true;
    res = check(n->get_child(0)) && res;
    res = check(n->get_child(2)) && res;
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    n->type = "undefined";
    // OP ==> BINOP ( ARG , ARG )
    if (n->num_children() > 4)
    {
        res = check(n->get_child(4)) && res;
        if (n->get_child(0)->type == "bool")
        {
            if (n->get_child(2)->type != "bool")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid argument type for Boolean operation: {}\n", input_file, n->get_child(2)->row, n->get_child(2)->col, n->get_child(2)->type);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
            }
            if (n->get_child(4)->type != "bool")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid argument type for Boolean operation: {}\n", input_file, n->get_child(4)->row, n->get_child(2)->col, n->get_child(4)->type);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
            }
            n->type = "bool";
            std::cout << std::string(depth * 2, ' ') << "OP->BINOP type bool\n";
            res = true && res;
            return res;
        }
        if (n->get_child(0)->type == "num")
        {
            if (n->get_child(2)->type != "num")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid argument type for Numeric operation: {}\n", input_file, n->get_child(2)->row, n->get_child(2)->col, n->get_child(2)->type);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
            }
            if (n->get_child(4)->type != "num")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid argument type for Numeric operation: {}\n", input_file, n->get_child(4)->row, n->get_child(4)->col, n->get_child(4)->type);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
            }
            n->type = "num";
            std::cout << std::string(depth * 2, ' ') << "OP->BINOP type num\n";
            res = true && res;
            return res;
        }
        if (n->get_child(0)->type == "compare")
        {
            if (n->get_child(2)->type != "num")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid argument type for Comparison operation: {}\n", input_file, n->get_child(2)->row, n->get_child(2)->col, n->get_child(2)->type);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
            }
            if (n->get_child(4)->type != "num")
            {
                std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid argument type for Comparison operation: {}\n", input_file, n->get_child(4)->row, n->get_child(4)->col, n->get_child(4)->type);
                std::cout << std::string(depth * 2, ' ') << t_error;
                this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n->get_child(2)));
            }
            n->type = "bool";
            std::cout << std::string(depth * 2, ' ') << "OP->BINOP type bool\n";
            res = true && res;
            return res;
        }
    }
    // OP ==> UNOP ( ARG )
    else if (n->get_child(0)->type == n->get_child(2)->type && (n->get_child(0)->type == "bool" || n->get_child(0)->type == "num"))
    {
        n->type = n->get_child(0)->type;
        std::cout << std::string(depth * 2, ' ') << "OP->UNOP type bool" << n->type << "\n";
        res = true && res;
    }
    if (n->type == "undefined")
    {
        std::string arg_types = fmt::format("( {}", n->get_child(2)->type);
        if (n->num_children() == 6)
        {
            arg_types = fmt::format("{}, {}", arg_types, n->get_child(4)->type);
        }
        arg_types.append(" )");
        std::string t_error = fmt::format("{}:{}:{} Type Error: Invalid operation type combination: {} {}\n", input_file, n->row, n->col, n->type, arg_types);
        std::cout << std::string(depth * 2, ' ') << t_error;
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, n));
        std::cout << "OP->UNOP type bool undefined\n";
        res = false;
    }
    return res;
}
bool Type_Checker::unop(std::shared_ptr<node> n)
{
    // std::cout << "Checking UNOP\n";
    if (n->get_child(0)->WORD == "not")
    {
        n->type = "bool";
    }
    else
    {
        n->type = "num";
    }
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    return true;
}
bool Type_Checker::binop(std::shared_ptr<node> n)
{
    // std::cout << "Checking BINOP\n";
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
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    return true;
}
bool Type_Checker::arg(std::shared_ptr<node> n)
{
    // std::cout << "Checking ARG\n";
    check(n->get_child(0));
    n->type = n->get_child(0)->type;
    return true;
}
bool Type_Checker::cond(std::shared_ptr<node> n)
{
    // std::cout << "Checking COND\n";
    bool res = check(n->get_child(0));
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    n->type = n->get_child(0)->type;
    return res;
}
bool Type_Checker::simple(std::shared_ptr<node> n)
{
    std::shared_ptr<node> op = n->get_child(0);
    std::shared_ptr<node> arg1 = n->get_child(2);
    std::shared_ptr<node> arg2 = n->get_child(4);
    // std::cout << "Checking SIMPLE\n";
    bool res = true;
    res = check(op) && res;
    res = check(arg1) && res;
    res = check(arg2) && res;
    if (op->type == arg1->type &&
        op->type == arg2->type &&
        op->type == "bool")
    {
        n->type = "bool";
        res = true && res;
    }
    res = check(arg2) && res;
    if (arg1->type == "num" &&
        arg1->type == arg2->type &&
        op->type == "compare")
    {
        n->type = "bool";
        res = true && res;
    }
    else
    {
        n->type = "undefined";
        res = false;
    }
    std::cout << std::string(depth * 2, ' ') << "SIMPLE type " << n->type << "\n";
    n->row = op->row;
    n->col = op->col;

    return res;
}
bool Type_Checker::composit(std::shared_ptr<node> n)
{
    // std::cout << "Checking COMPOSITE\n";
    bool res = true;
    res = check(n->get_child(0)) && res;
    res = n->get_child(0)->type == "bool" && res;
    res = check(n->get_child(2)) && res;
    res = n->get_child(2)->type == "bool" && res;
    if (n->num_children() > 4)
    {
        res = check(n->get_child(4)) && res;
        res = n->get_child(4)->type == "bool" && res;
        // std::cout << "2 ARG COMPOSIT type " << n->type;
    }
    if (!res)
    {
        n->type = "undefined";
    }
    else
    {
        n->type = "bool";
    }
    std::cout << std::string(depth * 2, ' ') << "COMPOSIT type " << n->type;
    n->row = n->get_child(0)->row;
    n->col = n->get_child(0)->col;
    return res;
}
bool Type_Checker::fname(std::shared_ptr<node> n)
{
    // std::cout << "Checking FNAME\n";
    n->type = type_of(n);
    return true;
}
bool Type_Checker::functions(std::shared_ptr<node> n)
{
    // std::cout << "Checking FUNCTIONS\n";
    bool res = true;
    res = check(n->get_child(0)) && res;
    std::cout << std::string(depth * 2, ' ') << "Result of checking DECL: " << (res ? "PASS" : "FAIL") << "\n";
    if (n->num_children() > 1)
    {
        res = check(n->get_child(1)) && res;
        std::cout << std::string(depth * 2, ' ') << "Result of checking recursive FUNCTIONS: " << (res ? "PASS" : "FAIL") << "\n";
    }
    return res;
}
bool Type_Checker::decl(std::shared_ptr<node> n)
{
    // std::cout << "Checking DECL\n";
    bool res = true;
    res = check(n->get_child(0)) && res;
    res = check(n->get_child(1)) && res;
    return res;
}
bool Type_Checker::header(std::shared_ptr<node> n)
{
    // std::cout << "Checking HEADER\n";
    check(n->get_child(1));
    bool res = true;
    check(n->get_child(3));
    check(n->get_child(5));
    check(n->get_child(7));
    // res = n->get_child(3)->type == "num" && res;
    // res = n->get_child(5)->type == "num" && res;
    // res = n->get_child(7)->type == "num" && res;
    std::shared_ptr<node> param = n->get_child(3);
    check(param);
    if (type_of(param) != "num")
    {
        std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
        std::cout << std::string(depth * 2, ' ') << t_error;
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
        res = false;
    }
    param = n->get_child(5);
    check(param);
    if (type_of(param) != "num")
    {
        std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
        std::cout << std::string(depth * 2, ' ') << t_error;
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
        res = false;
    }
    param = n->get_child(7);
    check(param);
    if (type_of(param) != "num")
    {
        std::string t_error = fmt::format("{}:{}:{} Invalid function parameter type: {}\n", input_file, param->row, param->col, type_of(param));
        std::cout << std::string(depth * 2, ' ') << t_error;
        this->type_errors.push_back(std::pair<std::string, std::shared_ptr<node>>(t_error, param));
        res = false;
    }
    return res;
}
bool Type_Checker::body(std::shared_ptr<node> n)
{
    // std::cout << "Checking BODY\n";
    bool res = true;
    for (auto c : n->get_children())
    {
        res = check(c) && res;
    }
    return res;
}
bool Type_Checker::prolog(std::shared_ptr<node> n)
{
    // std::cout << "Checking PROLOG\n";
    return true;
}
bool Type_Checker::epilog(std::shared_ptr<node> n)
{
    // std::cout << "Checking EPILOG\n";
    return true;
}
bool Type_Checker::locvars(std::shared_ptr<node> n)
{
    // std::cout << "Checking LOCVARS\n";
    check(n->get_child(1));
    check(n->get_child(4));
    check(n->get_child(7));
    return true; // already bound when binding scope
}
bool Type_Checker::subfuncs(std::shared_ptr<node> n)
{
    // std::cout << "Checking SUBFUNCS\n";
    return check(n->get_child(0));
}
bool Type_Checker::keyword(std::shared_ptr<node> n)
{
    // std::cout << "Checking KEYWORD\n";
    return true;
}
bool Type_Checker::literal(std::shared_ptr<node> n)
{
    // std::cout << "Checking LITERAL\n";
    n->type = type_of(n);
    return true;
}