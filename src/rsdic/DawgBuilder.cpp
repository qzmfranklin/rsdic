#include "DawgBuilder.h"

#include <functional>
#include <queue>
#include <stack>

/*
 * T is the letter type.
 */
class DawgBuilder::Node {
public:
    typedef uint16_t val_t; // value type
    friend DawgBuilder;

    Node(const val_t val): _val(val) {}

    void set_eow() { _is_eow = true; }
    const bool is_eow() const { return _is_eow; }

    void add_child(Node *p)
    {
        p->_parent = this;
        this->_child_list.push_back(p);
    }

    Node *find_child(const val_t val) const
    {
        for (const auto p: _child_list)
            if (p->_val == val)
                return p;
        return nullptr;
    }

    static void sort_child_list(Node *p)
    {
        std::sort(p->_child_list.begin(), p->_child_list.end(),
                [](Node *lhs, Node *rhs) { return lhs->_val < rhs->_val; });
    }

    /*
     * Visit nodes spawned by this node with the given visitor function
     *
     * Breadth first and depth first
     *
     * Caveat: If it is a tree, each node is only visited once. If it is a DAG,
     * no such guarantee yet (TODO)
     */
    void breadth_first_traverse(const std::function<void(Node*)> &fn)
    {
        std::queue<Node*> s;
        s.push(this);
        while(!s.empty()) {
            Node *curr = s.front();
            s.pop();
            for (auto p: curr->_child_list)
                s.push(p);
            fn(curr);
        }
    }

    void depth_first_traverse(const std::function<void(Node*)> &fn)
    {
        std::stack<Node*> s;
        s.push(this);
        while(!s.empty()) {
            Node *curr = s.top();
            s.pop();
            for (auto p: curr->_child_list)
                s.push(p);
            fn(curr);
        }
    }

    /*
     * Release memory starting from this node
     */
    static void release(Node *root)
    {
        std::stack<Node*> s;
        s.push(root);
        while(!s.empty()) {
            Node *curr = s.top();
            s.pop();
            for (const auto q: curr->_child_list)
                s.push(q);
            //if (curr->_parent)
                //std::remove(
                        //curr->_parent->_child_list.begin(),
                        //curr->_parent->_child_list.end(),
                        //curr);
            delete curr;
        }
    }

private:
    val_t _val = 0;
    bool _is_eow = false;
    Node *_parent = nullptr;
    std::vector<Node*> _child_list;
}; /* class DawgBuilder::Node */

DawgBuilder::~DawgBuilder()
{
    if (_root)
        Node::release(this->_root);
}

void DawgBuilder::make_root()
{
    this->_root = new Node(0);
}

void DawgBuilder::add_word(const std::string &&word)
{
    //fprintf(stderr,"    %s\n", word.c_str());

    // Find the first unmatched node
    Node *curr = _root;
    size_t i = 0;
    //fprintf(stderr,"1\n");

    for (i = 0; i < word.size(); i++) {
        //fprintf(stderr,"%c\n", word[i]);
        Node *next = curr->find_child(static_cast<Node::val_t>(word[i]));
        if (next)
            curr = next;
        else
            break;
    }

    // Complete the remaining string
    //fprintf(stderr,"1\n");
    for (; i < word.size(); i++) {
        //fprintf(stderr,"%c\n", word[i]);
        auto p = new Node(word[i]);
        curr->add_child(p);
        curr = p;
    }

    // Set the EndOfWord flag
    curr->set_eow();
}

void DawgBuilder::build()
{
    // Sort the child list by alphabetical order
    //this->_root->breadth_first_traverse(Node::sort_child_list);
}

std::string DawgBuilder::export_as_binary_string() const
{
    return std::string();
}

std::vector<std::string> DawgBuilder::export_all_words_debug() const
{
    std::vector<std::string> out;

    std::stack<const Node*> s;
    s.push(this->_root);
    while(!s.empty()) {
        const Node *curr = s.top();
        s.pop();
        for (const Node *p: curr->_child_list)
            s.push(p);
        if (curr->is_eow()) {
            std::string word;
            for (const Node *p = curr; p->_parent; p = p->_parent)
                word.insert(0, 1, static_cast<const char>(p->_val));
            //fprintf(stderr,"%s\n", word.c_str());
            out.push_back(std::move(word));
        }
    }

    std::sort(out.begin(), out.end());

    return out;
}
