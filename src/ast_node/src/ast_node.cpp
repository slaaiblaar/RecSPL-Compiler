#include "ast_node.hpp"
#include <fstream>
#include <regex>
int node_counter = 0; // Global UID counter

thread_local long node::node_id_counter = 0;
void node::clear_node()
{
    for (auto c : this->children)
    {
        c->clear_node();
    }
    this->children.clear();
    this->attempted_duplicates.clear();
    this->child_ids.clear();
    subtree_generated = false;
}
node::~node()
{
    std::cout << "DESTROYING NODE " << this->UID << std::endl;
}
std::string node::printftable(std::shared_ptr<node> n)
{
    std::string ftable = "";
    for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
    {
        ftable = fmt::format("{}{}() ==> {}, ", ftable, it->first, it->second[0]);
    }
    return ftable;
}
std::string node::printvtable(std::shared_ptr<node> n)
{
    std::string vtable = "";
    for (auto it = n->v_table.begin(); it != n->v_table.end(); ++it)
    {
        vtable = fmt::format("{}{}: {}, ", vtable, it->first, it->second);
    }
    return vtable;
}

std::string node::print_code(int depth, std::ofstream &code_file)
{
    auto c = this->get_children();
    std::string product;
    std::string indentation = std::string(depth * 4, ' ');
    // std::cout << std::string(depth * 2, ' ') << this->CLASS << ": " << this->WORD << "\n";
    if (this->CLASS == "PROGPRIMEPRIME")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "PROGPRIME")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "PROG")
    {
        std::string main_str = this->get_child(0)->print_code(depth, code_file);
        std::string globvars = "";
        std::string algo = "";
        std::string functions = "";
        int index = 1;

        if (this->get_child(1)->CLASS == "GLOBVARS")
        {
            globvars = this->get_child(1)->print_code(depth, code_file);
            algo = this->get_child(2)->print_code(depth, code_file);
            index = 2;
        }
        else
        {
            algo = this->get_child(1)->print_code(depth, code_file);
        }

        if (c.size() == index + 2)
        {
            functions = c[index + 1]->print_code(depth, code_file);
        }
        int num_insertions = 1; // main
        if (globvars.length() > 0)
        {
            ++num_insertions; // globvars
        }
        ++num_insertions; // algo
        if (functions.length() > 0)
        {
            ++num_insertions; // functions
        }

        std::string indentation = std::string(4, ' ');
        product = fmt::format("{}\n{}\n{}\n{}", main_str, globvars, (algo.length() > 0 ? indentation + algo : ""), functions);
    }
    else if (this->CLASS == "INSTRUC")
    {
        product = "{}{}{}\n";
        std::string indentation = std::string(depth * 4, ' ');
        std::string command = this->get_child(0)->print_code(depth, code_file);
        std::string semicolon = this->get_child(1)->print_code(depth, code_file);
        std::string instruc;
        int num_insertions = 3;
        if (c.size() > 2)
        {
            ++num_insertions;
            instruc = this->get_child(2)->print_code(depth, code_file);
        }
        if (num_insertions == 3)
        {
            product = fmt::format("{}{}{}\n", indentation, command, semicolon);
        }
        else
        {

            product = fmt::format("{}{}{}\n{}", indentation, command, semicolon, instruc);
        }
    }
    else if (this->CLASS == "COMMAND")
    {
        if (this->get_child(0)->CLASS == "FNAME")
        {
            std::string fname = this->get_child(0)->print_code(depth, code_file);
            std::string open = this->get_child(1)->print_code(depth, code_file);
            std::string atomic1 = this->get_child(2)->print_code(depth, code_file);
            std::string comma1 = this->get_child(3)->print_code(depth, code_file);
            std::string atomic2 = this->get_child(4)->print_code(depth, code_file);
            std::string comma2 = this->get_child(5)->print_code(depth, code_file);
            std::string atomic3 = this->get_child(6)->print_code(depth, code_file);
            std::string close = this->get_child(7)->print_code(depth, code_file);
            product = "{}{}{}{}{}{}{}{}";
            product = fmt::format("{}{}{}{}{}{}{}{}", fname, open, atomic1, comma1, atomic2, comma2, atomic3, close);
        }
        else if (this->get_child(0)->WORD == "return" || this->get_child(0)->WORD == "print")
        {
            std::string keyword = this->get_child(0)->print_code(depth, code_file);
            std::string atomic = this->get_child(1)->print_code(depth, code_file);
            product = "{}{}";
            product = fmt::format("{}{}", keyword, atomic);
        }
        else
        {
            std::string symbol = this->get_child(0)->print_code(depth, code_file);
            product = fmt::format("{}", symbol);
        }
    }
    else if (this->CLASS == "FUNCTIONS")
    {
        std::string decl = this->get_child(0)->print_code(depth, code_file);
        if (c.size() > 1)
        {
            std::string functions = this->get_child(1)->print_code(depth, code_file);
            product = fmt::format("{}\n{}", decl, functions);
        }
        else
        {
            product = fmt::format("{}\n", decl);
        }
    }
    else if (this->CLASS == "SUBFUNCS")
    {
        product = this->get_child(0)->print_code(depth + 1, code_file);
    }
    else if (this->CLASS == "DECL")
    {
        std::string head = this->get_child(0)->print_code(depth, code_file);
        std::string body = this->get_child(1)->print_code(depth, code_file);
        product = fmt::format("{}{}\n{}", indentation, head, body);
    }
    else if (this->CLASS == "VTYP" || this->CLASS == "PROLOG" || this->CLASS == "EPILOG")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "VNAME")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "FNAME")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "CONST")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "KEYWORD" || this->CLASS == "VID" || this->CLASS == "FID" || this->CLASS == "LITERAL")
    {
        product = fmt::format("{} ", this->WORD);
    }
    else if (this->CLASS == "ARG")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "ATOMIC")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "TERM")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "COND")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "GLOBVARS")
    {
        std::cout << std::string(depth * 2, ' ') << "printing GLOBVARS ==>";
        for (auto c : this->get_children())
        {
            std::cout << " " << c->WORD;
        }
        std::cout << "\n";
        std::string vtyp = this->get_child(0)->print_code(depth, code_file);
        std::string vname = this->get_child(1)->print_code(depth, code_file);
        std::string comma = this->get_child(2)->print_code(depth, code_file);
        std::string globvars = "";
        if (c.size() > 3)
        {
            globvars = this->get_child(3)->print_code(depth, code_file);
        }
        product = fmt::format("{}{}{}{}\n", vtyp, vname, comma, globvars);
    }
    else if (this->CLASS == "OP")
    {
        if (c.size() > 4)
        {
            std::string binop = this->get_child(0)->print_code(depth, code_file);
            std::string open = this->get_child(1)->print_code(depth, code_file);
            std::string arg1 = this->get_child(2)->print_code(depth, code_file);
            std::string comma = this->get_child(3)->print_code(depth, code_file);
            std::string arg2 = this->get_child(4)->print_code(depth, code_file);
            std::string close = this->get_child(5)->print_code(depth, code_file);
            product = fmt::format("{}{}{}{}{}{}", binop, open, arg1, comma, arg2, close);
        }
        else
        {
            std::string unop = this->get_child(0)->print_code(depth, code_file);
            std::string open = this->get_child(1)->print_code(depth, code_file);
            std::string arg = this->get_child(2)->print_code(depth, code_file);
            std::string close = this->get_child(3)->print_code(depth, code_file);
            product = fmt::format("{}{}{}{}", unop, open, arg, close);
        }
    }
    else if (this->CLASS == "ASSIGN")
    {
        std::string vname = this->get_child(0)->print_code(depth, code_file);
        if (c.size() == 2)
        {
            std::string input = this->get_child(1)->print_code(depth, code_file);
            product = fmt::format("{}{}", vname, input);
        }
        else if (c.size() == 3)
        {
            std::string assignment = this->get_child(1)->print_code(depth, code_file);
            std::string term = this->get_child(2)->print_code(depth, code_file);
            product = fmt::format("{}{}{}", vname, assignment, term);
        }
        else // 10
        {
            std::string assignment = this->get_child(1)->print_code(depth, code_file);
            std::string fname = this->get_child(2)->print_code(depth, code_file);
            std::string open = this->get_child(3)->print_code(depth, code_file);
            std::string atomic1 = this->get_child(4)->print_code(depth, code_file);
            std::string comma1 = this->get_child(5)->print_code(depth, code_file);
            std::string atomic2 = this->get_child(6)->print_code(depth, code_file);
            std::string comma2 = this->get_child(7)->print_code(depth, code_file);
            std::string atomic3 = this->get_child(8)->print_code(depth, code_file);
            std::string close = this->get_child(9)->print_code(depth, code_file);
            product = fmt::format("{}{}{}{}{}{}{}{}{}{}", vname, assignment, fname, open, atomic1, comma1, atomic2, comma2, atomic3, close);
        }
    }
    else if (this->CLASS == "COMPOSIT")
    {
        if (c.size() > 4)
        {
            std::string op = this->get_child(0)->print_code(depth, code_file);
            std::string open = this->get_child(1)->print_code(depth, code_file);
            std::string simple1 = this->get_child(2)->print_code(depth, code_file);
            std::string comma = this->get_child(3)->print_code(depth, code_file);
            std::string simple2 = this->get_child(4)->print_code(depth, code_file);
            std::string close = this->get_child(5)->print_code(depth, code_file);
            product = fmt::format("{}{}{}{}{}{}", op, open, simple1, comma, simple2, close);
        }
        else
        {
            std::string op = this->get_child(0)->print_code(depth, code_file);
            std::string open = this->get_child(1)->print_code(depth, code_file);
            std::string simple = this->get_child(2)->print_code(depth, code_file);
            std::string close = this->get_child(3)->print_code(depth, code_file);
            product = fmt::format("{}{}{}{}", op, open, simple, close);
        }
    }
    else if (this->CLASS == "SIMPLE")
    {
        std::string op = this->get_child(0)->print_code(depth, code_file);
        std::string open = this->get_child(1)->print_code(depth, code_file);
        std::string atomic1 = this->get_child(2)->print_code(depth, code_file);
        std::string comma = this->get_child(3)->print_code(depth, code_file);
        std::string atomic2 = this->get_child(4)->print_code(depth, code_file);
        std::string close = this->get_child(5)->print_code(depth, code_file);
        product = fmt::format("{}{}{}{}{}{}", op, open, atomic1, comma, atomic2, close);
    }
    else if (this->CLASS == "UNOP" || this->CLASS == "BINOP")
    {
        product = this->get_child(0)->print_code(depth, code_file);
    }
    else if (this->CLASS == "LOCVARS")
    {
        std::string vtyp1 = this->get_child(0)->print_code(depth, code_file);
        std::string vname1 = this->get_child(1)->print_code(depth, code_file);
        std::string comma1 = this->get_child(2)->print_code(depth, code_file);
        std::string vtyp2 = this->get_child(3)->print_code(depth, code_file);
        std::string vname2 = this->get_child(4)->print_code(depth, code_file);
        std::string comma2 = this->get_child(5)->print_code(depth, code_file);
        std::string vtyp3 = this->get_child(6)->print_code(depth, code_file);
        std::string vname3 = this->get_child(7)->print_code(depth, code_file);
        std::string comma3 = this->get_child(8)->print_code(depth, code_file);
        product = fmt::format("{}{}{}{}{}{}{}{}{}", vtyp1, vname1, comma1, vtyp2, vname2, comma2, vtyp3, vname3, comma3);
    }
    else if (this->CLASS == "HEADER")
    {
        std::string str = this->get_child(0)->print_code(depth, code_file);
        product = str;
        str = this->get_child(1)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(2)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(3)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(4)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(5)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(6)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(7)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
        str = this->get_child(8)->print_code(depth, code_file);
        product = fmt::format("{}{}", product, str);
    }
    // }
    else if (this->CLASS == "BODY")
    {
        std::string indentation = std::string((depth) * 4, ' ');
        std::string prolog = this->get_child(0)->print_code(depth, code_file);
        std::string locvars = this->get_child(1)->print_code(depth + 1, code_file);
        std::string algo = this->get_child(2)->print_code(depth + 1, code_file);
        std::string epilog = this->get_child(3)->print_code(depth, code_file);
        if (c.size() == 5)
        {
            std::string end = this->get_child(4)->print_code(depth, code_file);
            product = fmt::format("{}{}\n{}    {}\n{}\n{}{}\n{}{}", indentation, prolog, indentation, locvars, algo, indentation, epilog, indentation, end);
        }
        else
        {
            std::string subfuncs = this->get_child(4)->print_code(depth, code_file);
            std::string end = this->get_child(5)->print_code(depth, code_file);
            product = fmt::format("{}{}\n{}    {}\n{}\n{}{}\n{}\n{}{}", indentation, prolog, indentation, locvars, algo, indentation, epilog, subfuncs, indentation, end);
        }
    }
    else if (this->CLASS == "ALGO")
    {
        std::string begin = this->get_child(0)->print_code(depth, code_file);
        std::string end = c[c.size() - 1]->print_code(depth, code_file);
        std::string indentation = std::string(depth * 4, ' ');
        if (c.size() > 2)
        {
            std::string instruc = this->get_child(1)->print_code(depth + 1, code_file);

            product = fmt::format("{}{}\n{}{}{}", indentation, begin, instruc, indentation, end);
        }
        else
        {
            product = fmt::format("{}{}\n{}{}", indentation, begin, indentation, end);
        }
    }
    else if (this->CLASS == "BRANCH")
    {
        std::string _if = this->get_child(0)->print_code(depth, code_file);  // if
        std::string cond = this->get_child(1)->print_code(depth, code_file); // COND
        code_file << std::string(indentation, depth);
        std::string _then = this->get_child(2)->print_code(depth, code_file);      // then
        std::string t_algo = this->get_child(3)->print_code(depth + 1, code_file); // ALGO
        std::string _else = this->get_child(4)->print_code(depth, code_file);      // else
        std::string e_algo = this->get_child(5)->print_code(depth + 1, code_file); // ALGO
        std::string indentation = std::string(depth * 4, ' ');
        product = fmt::format("{}{}{}\n{}{}\n{}\n{}{}\n{}", indentation, _if, cond, indentation, _then, t_algo, indentation, _else, e_algo);
    }
    else
    {
        std::cerr << "\nPrinting not defined for class=" << this->CLASS << ", word=" << this->WORD << "\n";
        throw;
    }
    if (product.length() == 0)
    {
        std::cerr << "\n\nFailed to generate plaintext code\n\n";
        throw;
    }
    return product;
}
std::string escape_chars(std::string s)
{
    std::regex quotes("\"");
    std::string res = std::regex_replace(s, quotes, "\\\"");
    return res;
}
std::string node::printnode(int depth, std::string called_from)
{
    thread_local static std::unordered_map<int, bool> nodes_printed;
    thread_local static bool re_print_error = false;
    if (this->CLASS == "PROGPRIMEPRIME" || depth == 0)
    {
        nodes_printed.clear();
        re_print_error = false;
    }
    std::string root_indent = std::string(depth * 2, ' ');
    std::string product;
    std::string head = fmt::format("{}<{} word=\"{}\" uid=\"{}\" class=\"{}\" printed=\"{}\" node_id=\"{}\" prod=\"{}\">\n", root_indent, this->NAME, escape_chars(this->WORD), this->UID, this->CLASS, this->was_printed, std::to_string(this->node_id), this->prod);
    if (nodes_printed.find(this->UID) != nodes_printed.end())
    {
        std::cerr << head << fmt::format("Something went wrong, {}({}) was already printed\n\n", escape_chars(this->WORD), this->UID);
        throw;
        re_print_error = true;
        return fmt::format("{}{}  REPRINT ERROR\n{}</{}>\n", head, root_indent, root_indent, this->NAME);
    }
    nodes_printed[this->UID] = true;
    std::string inner_indent = std::string((depth + 1) * 2, ' ');
    std::string str_ftable = printftable(shared_from_this());
    str_ftable = fmt::format("{}<FTABLE>{}</FTABLE>\n", inner_indent, str_ftable);
    std::string str_vtable = printvtable(shared_from_this());
    str_vtable = fmt::format("{}<VTABLE>{}</VTABLE>\n", inner_indent, str_vtable);
    std::string chidren_str = "";
    if (this->children.size() > 0)
    {
        for (auto c : this->children)
        {
            if (c->was_printed)
                continue;
            std::string c_product = c->printnode(depth + 1, std::to_string(this->UID));

            chidren_str = fmt::format("{}{}", chidren_str, c_product);
            // unwind stack to trace reprint
            if (re_print_error)
                break;
        }
    }
    chidren_str = fmt::format("{}<CHILDREN>\n{}{}</CHILDREN>\n", inner_indent, chidren_str, inner_indent);
    product = fmt::format("{}{}{}{}{}</{}>\n", head, str_ftable, str_vtable, chidren_str, root_indent, escape_chars(this->WORD));
    return product;
}

node::node() : UID(node_counter++)
{
    node_id = node_id_counter++;
}

std::string printftable(std::shared_ptr<node> n)
{
    std::string ftable = "";
    for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
    {
        ftable = fmt::format("{}{}() ==> {}, ", ftable, it->first, it->second[0]);
    }
    return ftable;
}
std::string printvtable(std::shared_ptr<node> n)
{
    std::string vtable = "";
    for (auto it = n->v_table.begin(); it != n->v_table.end(); ++it)
    {
        vtable = fmt::format("{}{}: {}, ", vtable, it->first, it->second);
    }
    return vtable;
}

// // making a type alias because its super annoying to repeatedly define objects
// // of type std::shared_ptr<std::unordered_map<std::string, std::string>>
// Drill down through successive chains of FUNCTIONS == > FUNCTIONS productions void node::copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)
void node::copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            t->f_table[it->first][i] = it->second[i];
        }
    }
}
void node::copy_vtable(std::shared_ptr<vtable_type> f, std::shared_ptr<node> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        t->v_table[it->first] = it->second;
    }
}
void node::copy_ftable(std::shared_ptr<node> f, std::shared_ptr<node> t)
{
    for (auto it = f->f_table.begin(); it != f->f_table.end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            t->f_table[it->first][i] = it->second[i];
        }
    }
}
void node::copy_vtable(std::shared_ptr<node> f, std::shared_ptr<node> t)
{
    for (auto it = f->v_table.begin(); it != f->v_table.end(); ++it)
    {
        t->v_table[it->first] = it->second;
    }
}
void node::copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<ftable_type> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            (*t)[it->first][i] = it->second[i];
        }
    }
}