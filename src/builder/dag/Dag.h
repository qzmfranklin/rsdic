#ifndef __Dag__h__
#define __Dag__h__

#include <string>
#include <vector>

/*
 * This is not a general purpose DAG class. It is specifically used for building
 * compact dictionaries for fast retrieval.
 *
 * It has a super root that is the common ancestor of all other nodes in the
 * dag. This super root is made using make_root().
 *
 * Internally, it builds a tree first. Then you need to call
 * merge_common_suffixes() manually to merge common suffixes.
 */
class Dag {
public:
    ~Dag();
    void make_root();
    void add_word(const std::string &&);

    void merge_common_suffixes();

    std::string export_louds(const std::string &sep = "") const;
    std::string export_data() const;
    std::vector<std::string> export_sorted_wordlist_debug() const;
private:
    class Node;
    Node *_root = nullptr;
    std::vector<std::string> _wordlist;
};

#endif /* end of include guard __Dag__h__ */
