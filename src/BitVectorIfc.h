#ifndef __BitVector__BitVectorIfc__h__
#define __BitVector__BitVectorIfc__h__

#include <string>

class BitVectorIfc {
public:

    virtual ~BitVectorIfc() {}

    virtual bool get_bit(const uint64_t) const = 0;
    //virtual void set_bit(const uint64_t) = 0;
    //virtual void reset_bit(const uint64_t) = 0;
    virtual uint64_t rank0(const uint64_t) const = 0;
    virtual uint64_t rank1(const uint64_t) const = 0;
    virtual uint64_t select0(const uint64_t) const = 0;
    virtual uint64_t select1(const uint64_t) const = 0;

    virtual uint64_t size() const = 0;
    //virtual bool operator[](const uint64_t off) const { return get_bit(off); };
    //virtual std::string to_string() const = 0;
};

#endif /* end of include guard */
