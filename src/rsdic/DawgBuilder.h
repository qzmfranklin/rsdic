#ifndef __DawgBuilder__h__
#define __DawgBuilder__h__

#include <string>
#include <vector>

class DawgBuilder {
public:
    ~DawgBuilder();
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

#endif /* end of include guard __RsdicDawgBuilder__h__ */
