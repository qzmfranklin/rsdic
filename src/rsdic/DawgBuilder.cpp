#include "DawgBuilder.h"

#include <functional>
#include <stack>

/*
 * T is the letter type.
 */
class DawgBuilder::Node {
public:
    typedef uint16_t val_t; // value type

    Node(const val_t val): _val(val) {}

    void add_child(Node *p)
    {
        _child_list.push_back(p);
    }

    Node *find_child(const val_t val) const
    {
        for (const auto p: _child_list)
            if (p->_val == val)
                return p;
        return nullptr;
    }

    void sort_child_list()
    {
        std::sort(_child_list.begin(), _child_list.end(),
                [](Node *lhs, Node *rhs) { return lhs->_val < rhs->_val; });
    }

    void breath_first_traverse(std::function<void(Node*)>);

    static void release(Node *root)
    {
        std::stack<Node*> s;
        s.push(root);
        while(!s.empty()) {
            Node *p = s.top();
            s.pop();
            for (const auto q: p->_child_list)
                s.push(q);
            delete p;
        }
    }

private:
    val_t _val = 0;
    std::vector<Node*> _child_list;
}; /* class DawgBuilder::Node */

DawgBuilder::~DawgBuilder()
{
    if (_root)
        Node::release(this->_root);
}

void DawgBuilder::add_wordlist(const std::vector<std::string> &&wordlist)
{
    this->_wordlist = wordlist;
}

void DawgBuilder::build()
{
    /*
     * Step 0: Preprocess the wordlist to be unique and in this order:
     *          zyxz
     *          zyxy
     *          zyx
     *          zyw
     *          ...
     *          a
     */
    // This can be done on the command line very easily, at least for English


}

std::string DawgBuilder::export_as_binary_string() const
{
    return std::string();
}
