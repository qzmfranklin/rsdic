#include "Dag.h"

#include <functional>
#include <queue>
#include <stack>

class Dag::Node {
public:
    typedef char val_t; // value type
    friend Dag;

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
     * Set _color of all nodes to WHITE
     *
     * Pre-order, recursive implementation
     */
    void bleach()
    {
        _color = WHITE;
        for (auto p: _child_list)
            bleach(p);
    }

    /*
     * Visit nodes spawned by this node with the given visitor function
     *
     * Breadth first and depth first
     *
     * In order to make sure visit each node only once, we use a color flag
     * Node::_color:
     *          WHITE       untouched
     *          BLACK       stacked or queued
     */
    std::string breadth_first_traverse(const std::function<std::string (const Node*)> &fn)
    {
        std::string out;

        std::queue<const Node*> s;
        s.push(this);
        this->_color = BLACK;
        while(!s.empty()) {
            const Node *curr = s.front();
            s.pop();
            out += fn(curr);
            //curr->_color = BLACK;
            for (auto p: curr->_child_list)
                if (p->_color == WHITE) {
                    s.push(p);
                    p->_color = BLACK;
                }
        }

        this->bleach();

        return out;
    }

    std::string depth_first_traverse(const std::function<std::string(const Node*)> &fn)
    {
        std::string out;

        std::stack<const Node*> s;
        s.push(this);
        this->_color = BLACK;
        while(!s.empty()) {
            const Node *curr = s.top();
            s.pop();
            //curr->_color = BLACK;
            out += fn(curr);
            for (auto p: curr->_child_list)
                if (p->_color == WHITE) {
                    s.push(p);
                    p->_color = BLACK;
                }
        }

        this->bleach();

        return out;
    }

    /*
     * Release memory starting from the root node
     *
     * The root node is also released
     */
    static void release(Node *root)
    {
        for (Node* p: root->_parent_list)
            std::remove(p->_child_list.begin(), p->_child_list.end(), root);

        std::stack<Node*> s;
        s.push(root);
        root->_color = BLACK;
        while(!s.empty()) {
            Node *curr = s.top();
            s.pop();
            //s->_color = BLACK;
            for (const auto q: curr->_child_list)
                if (q->_color == WHITE) {
                    s.push(q);
                    q->_color = BLACK;
                }
            delete curr;
        }
    }

private:
    val_t _val = 0;
    enum { WHITE, BLACK } _color = WHITE;
    bool _is_eow = false;
    std::vector<Node*> _parent_list;
    std::vector<Node*> _child_list;
}; /* class Dag::Node */

Dag::~Dag()
{
    if (_root)
        Node::release(this->_root);
}

void Dag::make_root()
{
    this->_root = new Node(0);
}

void Dag::add_word(const std::string &&word)
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

std::string Dag::export_louds(const std::string &sep) const
{
    // Sort the child list by alphabetical order
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

std::string Dag::export_data() const
{
    // Sort the child list by alphabetical order
    //std::string tmp = _root->breadth_first_traverse([] (const Node *p) -> std::string {
    std::string tmp = _root->breadth_first_traverse([] (const Node *p) -> std::string {
        const size_t len = 200;
        char buf[len];
        size_t  offset = 0;
        offset += snprintf(buf + offset, len, "%X", p->_val);
        if (p->is_eow())
            offset += snprintf(buf + offset, len, "\tEOW");
        offset += snprintf(buf + offset, len, "\n");
        return std::string(buf);
    });
    tmp.pop_back(); // remove trailing '\n'
    return tmp;
}

std::string Dag::export_ascii_debug() const
{
    // Sort the child list by alphabetical order
    std::string tmp = _root->breadth_first_traverse([] (const Node *p) -> std::string {
        const size_t len = 256;
        size_t offset = 0;
        char buf[len];

        offset += snprintf(buf + offset, len, "%X", p->_val);
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

std::vector<std::string> Dag::export_sorted_wordlist_debug() const
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
