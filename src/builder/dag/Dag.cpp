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
        p->_parent_list.push_back(this);
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
     * Use the _color field with stack to verify that the directed graph
     * starting from this node is a dag
     *
     * Uses the theory that:
     *      A graph G, directed or not, is cyclic iff. a DFS yields a back edge
     *
     * In this function, _color is defined as:
     *          WHITE       untouched
     *          GREY        stacked
     *          BLACK       visited
     * With this definition, a back edge is an edge whose ending node is BLACK
     */
    bool verify_dag() const
    {
        std::stack<const Node*> s;
        s.push(this);
        this->_color = GREY;
        while(!s.empty()) {
            const Node *curr = s.top();
            s.pop();
            curr->_color = BLACK;
            for (const Node *p: curr->_child_list)
                switch (p->_color) {
                case WHITE:
                    s.push(p);
                    p->_color = GREY;
                    break;
                case GREY:
                    break;
                case BLACK:
                    return false;
                    break;
                }
        }
        return true;
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
     *
     * CAVEAT:
     *      This function assumes and does not check that the graph is acyclic.
     *      You need to call verify_dag() to verify that
     */
    std::string breadth_first_traverse(const std::function<std::string (const Node*)> &fn) const
    {
        std::string out;

        std::queue<const Node*> s;
        s.push(this);
        this->_color = BLACK;
        while(!s.empty()) {
            const Node *curr = s.front();
            s.pop();
            //printf("%c\n", curr->_val);
            out += fn(curr);
            //curr->_color = BLACK;
            for (auto p: curr->_child_list)
                if (p->_color == WHITE) {
                    s.push(p);
                    p->_color = BLACK;
                }
        }

        this->_bleach();

        return out;
    }

    std::string depth_first_traverse(const std::function<std::string(const Node*)> &fn) const
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

        this->_bleach();

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
    bool _is_eow = false;
    std::vector<Node*> _parent_list;
    std::vector<Node*> _child_list;

    mutable enum { WHITE, GREY, BLACK } _color = WHITE;

    /*
     * Set _color of all nodes to WHITE
     *
     * Pre-order, recursive implementation
     */
    void _bleach() const
    {
        _color = WHITE;
        for (const auto p: _child_list)
            p->_bleach();
    }
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

std::vector<std::string> Dag::export_sorted_wordlist_debug() const
{
    std::vector<std::string> out;

    std::string buf;
    buf.resize(BUFSIZ);

    std::stack<std::pair<const Node*, size_t>> s;
    s.push({this->_root, 0});
    buf[0] = _root->_val;
    _root->_color = Node::BLACK;
    while(!s.empty()) {
        const std::pair<const Node*, size_t> curr = s.top();
        s.pop();
        buf[curr.second - 1] = curr.first->_val;
        for (const auto p: curr.first->_child_list)
            if (p->_color == Node::WHITE) {
                s.push({p, curr.second + 1});
                p->_color = Node::BLACK;
            }
        if (curr.first->is_eow()) {
            std::string word(buf.data(), curr.second);
            //fprintf(stderr,"%s\n", word.c_str());
            out.push_back(std::move(word));
        }
    }

    std::sort(out.begin(), out.end());

    return out;
}
