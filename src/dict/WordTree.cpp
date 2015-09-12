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

WordTree::WordTree(const char *ptr, const size_t len)
{
    /*
     * _louds->load() copies the data.
     * rbx_open() allocates additional memory but does not copy the original
     * data (TODO: need to verify this)
     */
    _louds->load(ptr, len);
    const size_t offset = _louds->get_usage_bytes();
    _rbx_max_index = read_4bytes(ptr + offset);
    _rbx = rbx_open((const unsigned char*)ptr + offset + 4);
}

WordTree::~WordTree()
{
    // _louds releases its own copy of memory in its dtor
    rbx_close(_rbx);
}

bool WordTree::is_valid(const index_t off) const
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
    return bit ? _louds->select1((const uint32_t)rank0) : _louds->select1((const uint32_t)rank0 - 1);
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
    assert(off == 0);
    bool bit;
    uint64_t rank0;
    _louds->get_bit_and_rank0(off, &bit, &rank0);
    return is_valid(rank0 + 1) ? _louds->select0(rank0 + 1) - off : 0;
}

size_t WordTree::child_count_iterative(const index_t off) const
{
    return 0;
}

WordTree::index_t WordTree::next_unsafe(const index_t off) const
{
    uint64_t rank0 = _louds->rank0(off);
    return _louds->select0(rank0 + 1) + 1;
}

WordTree::index_t WordTree::find(const std::string&) const
{
    index_t retval = 0;

    std::stack<index_t> s;
    s.push(0); // super root
    while(!s.empty()) {
        const index_t curr = s.top();
        s.pop();
    }

    return retval;
}

WordTree::rbx_index_t WordTree::_get_rbx_index(const index_t off) const
{
    return _louds->rank0(off);
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
