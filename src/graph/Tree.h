#ifndef __Tree__h__
#define __Tree__h__

#include <string>
#include <vector>

/*
 * This is a playground class for testing the Dag class. I believe that I had
 * enough experience to move on to the more complicated Dag class now that I
 * have finished the Tree class.
 */
class Tree {
public:
    ~Tree();
    void make_root();
    void add_word(const std::string &&);

    std::string export_louds(const std::string &sep = "") const;
    std::string export_data() const;

    std::string export_ascii_debug() const;
    std::vector<std::string> export_sorted_wordlist_debug() const;
private:
    class Node;
    Node *_root = nullptr;
    std::vector<std::string> _wordlist;
};

#endif /* end of include guard __Tree__h__ */
