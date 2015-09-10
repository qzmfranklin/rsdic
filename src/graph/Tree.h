#ifndef __Tree__h__
#define __Tree__h__

#include <string>
#include <vector>

namespace graph {

/*
 * Running example of usage in gtest_Tree.cpp
 * Brief example of usage:
 *
 *      Tree g;
 *      g.make_root();
 *      for_each(const std::string &&keyword)
 *          g.add_word(keyword);
 *
 *      // The LOUDS is exported as a binary string, i.e.:
 *      //          "01000111..."
 *      g.export_louds("\n");
 *
 *      // The actual data associated with each node in LOUDS
 *      //          54\n
 *      //          AF\tEOW\n
 *      //          59\n
 *      //          5A\n
 *      //          A9\tEOW\n
 *      g.export_data();
 *
 * Rationale:
 *      Some people may find this string based interface annoying
 *
 *      Well, it has its own advantages:
 *          1.  Minimal dependency, easy to drop into your own projects
 *          2.  Easy to debug because outputs are human readable strings
 *          3.  Good compatibility because outputs are made ASCII
 *          4.  Easy to parse the TSV output
 *          5.  Traditional *nix tools apply
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

} /* namespace graph */

#endif /* end of include guard __Tree__h__ */
