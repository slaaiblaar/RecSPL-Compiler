#include "automata.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <iterator>

int RegexpNode::uidCounter = 0;
RegexpNode::RegexpNode(char value)
{
    this->value = value;
    this->uid = uidCounter++;
    switch (value)
    {
    case '|':
        precedence = 1;
        break;
    case '*':
        precedence = 3;
        break;
    case '(':
        precedence = 4;
        break;
    default:
        // normal input if not an operator
        // explicit concatenation operator gets added in the tree constructor
        // after a normal input node is created
        precedence = 5;
    }
}
RegexpNode::~RegexpNode()
{
    // std::cout << "Destroying [" << value << "](" << precedence << ")" << "-" << uid << std::endl;
}
void RegexpNode::free()
{
    // std::cout << "Freeing [" << value << "](" << precedence << ")" << "-" << uid << std::endl;
    for (int i = 0; i < children.size(); i++)
    {
        children[i]->free();
    }
    children.clear();
    parent = nullptr;
}

void RegexpNode::addChild(std::shared_ptr<RegexpNode> child)
{
    children.push_back(child);
    child->parent = shared_from_this();
}
void RegexpNode::printNode(int depth, int offset)
{
    if (depth > 1000 || offset > 1000)
    {
        throw "A node references an ancestor";
    }
    for (int i = 0; i < offset; i++)
    {
        std::cout << "  ";
    }
    for (int i = 0; i < depth; i++)
    {
        std::cout << "|_";
    }
    std::cout << "[" << value << "](" << precedence << ")-" << uid << std::endl;
    for (int i = 0; i < children.size(); ++i)
    {
        children[i]->printNode(depth + 1, offset);
    }
}

void RegexpNode::flattenNode()
{
    if (children.size() == 0)
    {
        return;
    }
    std::vector<std::shared_ptr<RegexpNode>> newChildren;
    // std::cout << "Flattening node: " << value << std::endl;
    for (int i = 0; i < children.size(); i++)
    {
        children[i]->flattenNode();
        if (children[i]->precedence < 3 && children[i]->children.size() == 1)
        {
            newChildren.push_back(children[i]->children[0]);
            children[i]->children[0]->parent = shared_from_this();
        }
        else if (children[i]->value == value && children[i]->precedence == precedence)
        {
            for (int j = 0; j < children[i]->children.size(); j++)
            {
                newChildren.push_back(children[i]->children[j]);
                children[i]->children[j]->parent = shared_from_this();
            }
            children[i]->children.clear();
        }
        else
        {
            newChildren.push_back(children[i]);
        }
    }
    children = newChildren;
}

/*
    Operator Node values:
        |  with prec 1 - Alternation
        *  with prec 3 - Duplication
        .  with prec 2 - Concatenation
        () Grouping represented implicitly by tree structure

        Concatenation will differentiated from decimal point by precedence
*/
RegexpTree::RegexpTree(std::string input, int subtreeDepth)
{
    std::cout << "Creating tree" << std::endl;
    this->input = input;
    if (input.length() == 0)
    {
        std::cout << "Empty input" << std::endl;
        throw "Empty input";
    }
    root = std::make_shared<RegexpNode>('.');
    root->precedence = 2;
    std::shared_ptr<RegexpNode> curr = root;
    std::shared_ptr<RegexpNode> prev;

    for (int i = 0; i < input.length(); i++)
    {

        prev = curr;
        curr = std::make_shared<RegexpNode>(input[i]);
        if (curr->value == '(')
        {
            int grpLen = groupLength(i, subtreeDepth);
            RegexpTree groupSubtree(input.substr(i + 1, grpLen), subtreeDepth + 1);
            curr->addChild(groupSubtree.root);
            i += grpLen + 1;
            curr->value = '.';
            curr->precedence = 2;
            groupSubtree.root = nullptr;
        }
        else if (curr->precedence == 5)
        {
            std::shared_ptr<RegexpNode> concat = std::make_shared<RegexpNode>('.');
            concat->precedence = 2;
            concat->addChild(curr);
            curr = concat;
        }
        // while previous operator exists and binds more tightly than current operator
        while (prev->precedence > curr->precedence && prev->parent != nullptr)
        {
            prev = prev->parent;
        }
        /*
            If the previous operator binds more tightly than the current
            operator, or if the current operator is the duplicate operator, then
            the current operator should be the parent of the previous operator.

            Second condition added to prevent the tree:   * (prev \w prec 3)
                (a*)* = a*                              /   \
                                                       x     * (curr \w prec 3)
        */
        if (prev->precedence > curr->precedence || curr->value == '*')
        {
            curr->parent = prev->parent;
            curr->children.insert(curr->children.begin(), prev);
            prev->parent = curr;
            if (curr->parent != nullptr)
            {
                // find prev in parent's children
                for (int j = 0; j < curr->parent->children.size(); ++j)
                {
                    if (curr->parent->children[j] == prev)
                    {
                        curr->parent->children[j] = curr;
                        break;
                    }
                }
            }
            if (curr->parent == nullptr)
            {
                root = curr;
            }
        }
        else if (prev->precedence <= curr->precedence)
        {
            prev->addChild(curr);
        }
        // for (int indent = 0; indent < subtreeDepth; ++indent)
        // {
        //     std::cout << "  ";
        // }
        // std::cout << "Current Tree: \n";
        // root->printNode(0, subtreeDepth);
    }

    root->flattenNode();
    // std::cout << "Input: " << std::endl;
    // std::cout << input << std::endl;
    // std::cout << "Flattened tree" << std::endl;
    // root->printNode(0, 0);
}
RegexpTree::~RegexpTree()
{
    // std::cout << "Freeing tree" << std::endl;
    if (root != nullptr)
    {
        root->free();
    }
    root = nullptr;
}
int RegexpTree::groupLength(int index, int offset)
{
    int start = index;
    int count = 0;
    do
    {
        if (input[index] == '(')
        {
            count++;
        }
        else if (input[index] == ')')
        {
            count--;
        }
        index++;
    } while (index < input.length() && count != 0);
    if (count != 0 && index == input.length())
    {
        std::cout << "Unbalanced parentheses" << std::endl;
        std::cout << "input: " << input << std::endl;
        std::cout << "Start:End " << start << ":" << index << std::endl;
        std::cout << count << std::endl;
        std::cout << input.substr(start, index - start) << std::endl;
        throw "Unbalanced parentheses";
    }
    return index - 2 - start;
}
void RegexpTree::concatenateTree(std::shared_ptr<RegexpTree> subTree)
{
    std::shared_ptr<RegexpNode> newRoot = std::make_shared<RegexpNode>('|');
    newRoot->addChild(root);
    newRoot->addChild(subTree->root);
    root = newRoot;
    subTree->root = nullptr;
    root->flattenNode();
    printTree();
}
void RegexpTree::printTree()
{
    root->printNode(0, 0);
}