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
    typedef uint32_t WordTree::index_t;

    WordTree(const void *ptr, const size_t len);

    bool good() const { return _state == READY; }

    /*
     * Test that a node
     *      is a valid node
     *      is an end-of-word node
     */
    bool is_valid(const index_t) const;
    bool is_eow(const index_t) const;

    /*
     * Return the first bit of the last node of the given string.
     * Return 0 if the string is not found.
     *
     * This function is very slow. It is mainly used for debugging purposes.
     * Please do NOT use this function in performance critical code regions.
     */
    index_t find(const std::string&) const;

    /*
     * Return the UTF8 code unit associated with the node.
     */
    char utf8unit(const index_t) const;

    /*
     * Return the first bit of the parent, if any.
     * Return 0 otherwise.
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
     * Get the next node in a depth first traversal.
     *
     * Return 0 if:
     *      This node is already a last child, as verified by is_last_child().
     *
     * If this node has the next sibling node, the return value is the same as
     * next_sibling().
     */
    index_t next(const index_t) const;

    /*
     * Get the next sibling node, if any.
     *
     * Return 0 if:
     *      This node is already a last child, as verified by is_last_child().
     */
    index_t next_sibling(const index_t) const;
private:
    enum { EMPTY, READY } _state = EMPTY;

    std::shared_ptr<rsdic::Rsdic> _louds = nullptr;
    size_t _rbx_max_index = 0;
    struct *rbx = nullptr;
};

} /* namespace dict */

#endif /* end of include guard  */
