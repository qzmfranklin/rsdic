#include "WordTree.h"
#include <stack>
#include <assert.h>

namespace dict {

// TODO: Endianness is a safety hole here.
static uint32_t read_4bytes(const void *buf) {
    const uint8_t *ptr = reinterpret_cast<const uint8_t*>(buf);
    const uint32_t tmp0 = ptr[0];
    const uint32_t tmp1 = ptr[1];
    const uint32_t tmp2 = ptr[2];
    const uint32_t tmp3 = ptr[3];
    return (tmp0 << 24u) | (tmp1 << 16u) | (tmp2 << 8u) | tmp3;
}

WordTree::WordTree(const char *ptr, const size_t len):
    _louds(std::make_shared<rsdic::Rsdic>())
{
    /*
     * _louds->load() copies the data.
     * rbx_open() allocates additional memory but does not copy the original
     * data (TODO: need to verify this)
     */
    const size_t offset = _louds->load(ptr, len);
    _rbx_max_index = read_4bytes(ptr + offset);
    _rbx = rbx_open((const unsigned char*)ptr + offset + 4);
}

WordTree::~WordTree()
{
    // _louds releases its own copy of memory in its dtor
    rbx_close(_rbx);
}

bool WordTree::is_valid_rbx_index(const rbx_index_t off) const
{
    return off <= _rbx_max_index;
}

bool WordTree::is_eow(const index_t off) const
{
    return _get_flag_byte(off) & static_cast<uint8_t>(BitMask::EndOfWord);
}

char WordTree::get_data(const index_t off) const
{
    return _get_data_byte(off);
}

// Given the bit offset m in the bit vector v:
//      v[m] = 1:
//          child(m)  = select0(rank1(m)) + 1
//          parent(m) = select1(rank0(m))
//      v[m] = 0:
//          no child node
//          parent(m) = select1(rank0(m) - 1)

WordTree::index_t WordTree::parent(const index_t off) const
{
    bool bit;
    uint64_t rank0;
    _louds->get_bit_and_rank0(off, &bit, &rank0);
    if (rank0 == 0)
        return 0;
    return _louds->select1(bit ? rank0 : rank0 - 1);
}

WordTree::index_t WordTree::child(const index_t off) const
{
    bool bit;
    uint64_t rank1;
    _louds->get_bit_and_rank1(off, &bit, &rank1);
    return bit ? _louds->select0((const uint32_t)rank1) + 1 : 0;
}

size_t WordTree::child_count(const index_t off) const
{
    bool bit;
    uint64_t rank0;
    _louds->get_bit_and_rank0(off, &bit, &rank0);
    if (!bit)
        return 0;
    rbx_index_t next = rank0 + 1;
    return is_valid_rbx_index(next) ? _louds->select0(next) - off : 0;
}

size_t WordTree::child_count_iterative(const index_t off) const
{
    index_t curr = off;
    while(_louds->get_bit(curr))
        curr++;
    return curr - off;
}

WordTree::index_t WordTree::next_unsafe(const index_t off) const
{
    uint64_t rank0 = _louds->rank0(off);
    return _louds->select0(rank0 + 1) + 1;
}

WordTree::index_t WordTree::find(const std::string &key) const
{
    struct Node {
        Node(index_t index_, size_t depth_): index(index_), depth(depth_) {}
        index_t index = 0;
        int depth = 0;
    };

    const size_t key_size = key.size();

    std::stack<Node> s;

    { // Push children of the super root
        const size_t num = child_count(0);
        for(int i = 0; i < num; i++)
            s.push(Node(child(i), 1));
    }

    // Core loop
    while(!s.empty()) {
        const Node curr = s.top();
        s.pop();

        if (curr.depth > key_size)
            continue;

        const rbx_index_t rbxoff = _get_rbx_index(curr.index);
        const char *buf;
        int len;
        buf = (const char*)rbx_get(_rbx, rbxoff, &len);
        assert(buf);
        assert(len);
        const uint8_t bitmask = len > 1 ? buf[1] : 0;

        const char ch = static_cast<char>(buf[0]);
        if (ch != key[curr.depth - 1])
           continue;

        if (curr.depth == key_size) {
            // Terminate if this is the last char.
            if (bitmask & BitMask::EndOfWord)
                return curr.index;
            else
                return 0;
        } else {
            // Push children.
            const size_t num = child_count(curr.index);
            for(int i = 0; i < num; i++)
                s.push(Node(child(curr.index + i), curr.depth + 1));
        }
    }

    return 0;
}

void WordTree::_inspect_bit_debug(const char *action, const index_t index) const
{
    const rbx_index_t rbxoff = _get_rbx_index(index);
    const uint8_t data = _get_data_byte(rbxoff);
    fprintf(stderr,"%s\t%8u %02X %c\n", action, rbxoff, data, (char)data);
    /*
     *fprintf(stderr,"%s\t%8u %c %02X %c %u\n"
     *        , action
     *        , rbxoff
     *        , _louds->get_bit(index) ? '1' : '0'
     *        , data
     *        , (char)data
     *        , index
     *        );
     */
}

std::vector<std::string> WordTree::get_all_words() const
{
    std::vector<std::string> out;

    std::stack<index_t> s;

    { // Push children of the super root
        //_inspect_bit_debug("POP", 0);
        const size_t num = _louds->select0(1);
        for(int i = 0; i < num; i++) {
            const index_t child = this->child(i);
            //_inspect_bit_debug("PUSH", child);
            s.push(child);
        }
    }

    // Core loop
    while(!s.empty()) {
        const index_t curr = s.top();
        s.pop();
        //fprintf(stderr,"\n");
        //_inspect_bit_debug("POP", curr);

        const rbx_index_t rbxoff = _get_rbx_index(curr);
        const char *buf;
        int len;
        buf = (const char*)rbx_get(_rbx, rbxoff, &len);
        assert(buf);
        assert(len);
        const uint8_t bitmask = len > 1 ? buf[1] : 0;

        const index_t last_bit_of_super_root = _louds->select0(1);
        if (bitmask & BitMask::EndOfWord) {
            std::string tmp;
            for (index_t i = curr; i > last_bit_of_super_root; i = parent(i)) {
                const rbx_index_t rbxoff = _get_rbx_index(i);
                const char ch = static_cast<char>(_get_data_byte(rbxoff));
                tmp.insert(0, 1, ch);
            }
            //printf("%s\n", tmp.c_str());
            out.push_back(std::move(tmp));
        }

        // Push children.
        const size_t num = child_count(curr);
        for(index_t i = 0; i < num; i++) {
            const index_t index = child(curr + i);
            //_inspect_bit_debug("PUSH", index);
            s.push(index);
        }
    }

    std::sort(out.begin(), out.end());

    return out;
}

void WordTree::dump_louds_debug() const
{
    assert(_louds);
    _louds->dump_debug();
}

WordTree::rbx_index_t WordTree::_get_rbx_index(const index_t off) const
{
    bool bit;
    uint64_t rank0;
    _louds->get_bit_and_rank0(off, &bit, &rank0);
    return bit ? rank0 : rank0 - 1;
}

uint8_t WordTree::_get_data_byte(const rbx_index_t rbxoff) const
{
    const char *buf;
    int len;
    buf = (const char*)rbx_get(_rbx, rbxoff, &len);
    return buf[0];
}

uint8_t WordTree::_get_flag_byte(const rbx_index_t rbxoff) const
{
    const char *buf;
    int len;
    buf = (const char*)rbx_get(_rbx, rbxoff, &len);
    return len > 1 ? buf[0] : 0;
}

} /* namespace dict */
