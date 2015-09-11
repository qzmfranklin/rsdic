#include "Tree.h"

#include <functional>
#include <queue>
#include <stack>

#include <assert.h>

namespace builder {

class Tree::Node {
public:
    typedef char val_t; // value type
    friend Tree;

    Node(const val_t val): _val(val) {}

    void set_eow() { _is_eow = true; }
    bool is_eow() const { return _is_eow; }

    bool is_last_child() const
    {
        if (!_parent)
            return true;
        assert(!_parent->empty());
        assert(_parent->back());
        return this == _parent->_child_list.back();
    }

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
    std::string breadth_first_traverse(const std::function<std::string (const Node*)> &fn)
    {
        std::string out;

        std::queue<const Node*> s;
        s.push(this);
        while(!s.empty()) {
            const Node *curr = s.front();
            s.pop();
            out += fn(curr);
            for (auto p: curr->_child_list)
                s.push(p);
        }

        return out;
    }

    std::string depth_first_traverse(const std::function<std::string(const Node*)> &fn)
    {
        std::string out;

        std::stack<const Node*> s;
        s.push(this);
        while(!s.empty()) {
            const Node *curr = s.top();
            s.pop();
            out += fn(curr);
            for (auto p: curr->_child_list)
                s.push(p);
        }

        return out;
    }

    /*
     * Release memory starting from this node
     */
    static void release(Node *root)
    {
        if (root->_parent)
            std::remove(root->_parent->_child_list.begin(), root->_parent->_child_list.end(), root);
        std::stack<Node*> s;
        s.push(root);
        while(!s.empty()) {
            Node *curr = s.top();
            s.pop();
            for (const auto q: curr->_child_list)
                s.push(q);
            delete curr;
        }
    }

private:
    val_t _val = 0;
    bool _is_eow = false;
    Node *_parent = nullptr;
    std::vector<Node*> _child_list;
}; /* class Tree::Node */

Tree::~Tree()
{
    if (_root)
        Node::release(this->_root);
}

void Tree::make_root()
{
    this->_root = new Node(0);
}

void Tree::add_word(const std::string &&word)
{
    //fprintf(stderr,"    %s\n", word.c_str());
    assert(this->_root);

    // Find the first unmatched node
    Node *curr = _root;
    size_t i = 0;
    //fprintf(stderr,"1\n");

    for (i = 0; i < word.size(); i++) {
        //fprintf(stderr,"%02X\n", static_cast<const uint8_t>(word[i]));
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

std::string Tree::export_louds(const std::string &sep) const
{
    assert(this->_root);
    return _root->breadth_first_traverse([&sep] (const Node *p) -> std::string {
        const size_t len = 200;
        size_t offset = 0;
        char buf[len];

        const size_t num_child = p->_child_list.size();
        for (size_t i = 0; i < num_child; i++)
            offset += snprintf(buf + offset, len, "1");
        offset += snprintf(buf + offset, len, "0");

        return std::string(buf) + sep;
    });
}

std::string Tree::export_data() const
{
    assert(this->_root);
    std::string tmp = _root->breadth_first_traverse([] (const Node *p) -> std::string {
        const size_t len = 200;
        char buf[len];
        size_t  offset = 0;
        offset += snprintf(buf + offset, len, "%02X", static_cast<const uint8_t>(p->_val));
        if (p->is_last_child())
            offset += snprintf(buf + offset, len, "\tLAST_CHILD");
        if (p->is_eow())
            offset += snprintf(buf + offset, len, "\tEND_OF_WORD");
        offset += snprintf(buf + offset, len, "\n");
        return std::string(buf);
    });
    tmp.pop_back(); // remove trailing '\n'
    return tmp;
}

std::string Tree::export_ascii_debug() const
{
    assert(this->_root);
    std::string tmp = _root->breadth_first_traverse([] (const Node *p) -> std::string {
        const size_t len = 256;
        size_t offset = 0;
        char buf[len];

        offset += snprintf(buf + offset, len, "%X", static_cast<const uint8_t>(p->_val));
        offset += snprintf(buf + offset, len, "\t");
        const size_t num_child = p->_child_list.size();
        for (size_t i = 0; i < num_child; i++)
            offset += snprintf(buf + offset, len, "1");
        offset += snprintf(buf + offset, len, "0");
        offset += snprintf(buf + offset, len, "\n");

        return std::string(buf);
    });
    tmp.pop_back();
    return tmp;
}

std::vector<std::string> Tree::export_sorted_wordlist_debug() const
{
    std::vector<std::string> out;

    std::stack<const Node*> s;
    s.push(this->_root);
    while(!s.empty()) {
        const Node *curr = s.top();
        s.pop();
        for (const auto p: curr->_child_list)
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

} /* namespace builder */
