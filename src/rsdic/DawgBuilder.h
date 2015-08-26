#ifndef __DawgBuilder__h__
#define __DawgBuilder__h__

#include <string>
#include <vector>

class DawgBuilder {
public:
    ~DawgBuilder();
    void add_wordlist(const std::vector<std::string> &&wordlist);
    void build();
    std::string export_as_binary_string() const;
private:
    class Node;
    Node *_root = nullptr;
    std::vector<std::string> _wordlist;
};

#endif /* end of include guard __RsdicDawgBuilder__h__ */
