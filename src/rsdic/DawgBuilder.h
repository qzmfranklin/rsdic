#ifndef __DawgBuilder__h__
#define __DawgBuilder__h__

#include <string>
#include <vector>

class DawgBuilder {
public:
    ~DawgBuilder();
    void make_root();
    void add_word(const std::string &&);
    void build();
    std::string export_as_binary_string() const;

    std::vector<std::string> export_all_words_debug() const;
private:
    class Node;
    Node *_root = nullptr;
    std::vector<std::string> _wordlist;
};

#endif /* end of include guard __RsdicDawgBuilder__h__ */
