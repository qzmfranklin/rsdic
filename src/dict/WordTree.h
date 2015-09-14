#ifndef __dict__WordTree__h__
#define __dict__WordTree__h__

#include "src/builder/tree/Tree.h"
#include "src/os/path.h"
#include "src/succinct/rsdic/Rsdic.h"
#include "src/succinct/rx/rx.h"

namespace dict {

class WordTree {
public:
    /*
     * WordTree nodes are addressed with this opaque type index_t. The WordTree
     * APIs operate with index_t.
     *
     * Given bit vector v:
     *          X = 0 or 1
     *      rankX(m)     number of X's in [0, m]
     *      selectX(m)   first offset in v s.t. whose rankX() is m
     *
     * CAVEAT:
     *      The definitions of rank and select used here are slightly different
     *      from the one used by the original rsdic:: classes. You need to
     *      understand this if you want to understand the internals of this
     *      class.
     *
     *
     * Internally, index_t is the offset in the bit vector. Some important
     * implication is:
     *
     *      1.  rank0(index_t) is the offset in the rbx array.
     *
     *      2.  Because LOUDS uses in general multiple bits to represent nodes,
     *          it is possible to have several adjacent indices pointing to the
     *          different bits of the same node. But that is not the level of
     *          detail this structure want to expose. Therefore, when we refer
     *          to a node by its index_t, we always assume that the index points
     *          to the first bit of the node in the LOUDS.
     *
     *      3.  index_t is unsigned because we really do not need negative
     *          numbers for anything. The super root, i.e, the 0-th node, means
     *          'nothing' or 'error', whichever proper in the context.
     *
     * Implementation of functions are documented in the cpp file.
     */
    typedef uint32_t index_t;
    typedef uint32_t rbx_index_t;

    WordTree(const char *ptr, const size_t len);
    ~WordTree();

    /*
     * Test that a node
     *      is a valid rbx index
     *      is an end-of-word node
     */
    bool is_valid_rbx_index(const index_t) const;
    bool is_eow(const index_t) const;

    /*
     * Return the UTF8 code unit associated with the node.
     */
    char get_data(const index_t) const;

    /*
     * Return the first bit of the parent, if any.
     * Return 0 otherwise.
     *
     * CAVEAT: Do not call this function with index_t 0, i.e., the super root.
     */
    index_t parent(const index_t) const;

    /*
     * Return the first bit of the child, if any.
     * Return 0 if this node does not have children, or this bit is 0.
     *
     * For example, a node '1110'. An index pointing to the second '1' leads to
     * the second child. An index pointing to the trailing '0' does not lead to
     * any child.
     */
    index_t child(const index_t) const;

    /*
     * select0(rank0(off) + 1) - off
     * Abort if current bit is 0
     * Can lead to unexpected behavior if out of range
     */
    size_t child_count(const index_t) const;

    /*
     * Count get_bit(off) until the next 0
     */
    size_t child_count_iterative(const index_t) const;

    /*
     * select0(rank0(off) + 1) + 1
     * Unsafe if current bit
     *      1.  is 0, or
     *      2.  is already the last child of its parent, or
     *      3.  is the last bit in the bit vector (for LOUDS, this is implied by 2)
     */
    index_t next_unsafe(const index_t) const;

    /*
     * Add proper checking to next_sibling_unsafe().
     * Return 0 if any such checking fails.
     */
    //index_t next_sibling_safe(const index_t) const;

    /*
     * select0(rank0(off) + 1)
     * Subject to the same unsafeness as next_sibling_unsafe().
     *
     * This function can be very useful fora
     */
    //index_t last_bit_unsafe(const index_t) const;

    /*
     * Return the first bit of the last node of the given string.
     * Return 0 if the string is not found.
     *
     * This function is very slow. It is mainly used for debugging purposes.
     * Please do NOT use this function in performance critical code regions.
     */
    index_t find(const std::string&) const;

private:
    std::shared_ptr<rsdic::Rsdic> _louds = nullptr;
    uint32_t _rbx_max_index = 0;
    struct rbx *_rbx = nullptr;

    enum BitMask: uint8_t {
        LastChild = 0x1,
        EndOfWord = 0x1 << 1
    };

    rbx_index_t _get_rbx_index(const index_t) const;
    uint8_t _get_data_byte(const rbx_index_t) const;
    uint8_t _get_flag_byte(const rbx_index_t) const;
};

} /* namespace dict */

#endif /* end of include guard  */
