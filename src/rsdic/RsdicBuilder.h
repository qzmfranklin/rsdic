/*
 *  Copyright (c) 2012 Daisuke Okanohara
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#ifndef RSDIC_RSDIC_BUILDER_HPP_
#define RSDIC_RSDIC_BUILDER_HPP_

#include "Type.h"
#include <vector>
#include <stdint.h>

namespace rsdic{

class Rsdic;
class RsdicBuilder{
public:
    void clear();
    void push_back(const bool bit);
    void add_string(const std::string &str);
    Rsdic build();

private:
    enum {
        EMPTY,
        READY
    } _state = EMPTY;

    void _write_block();

    std::vector<uint64_t> _bits;
    std::vector<rsdic_uint> _pointer_blocks;
    std::vector<rsdic_uint> _select_one_inds;
    std::vector<rsdic_uint> _select_zero_inds;
    std::vector<rsdic_uint> _rank_blocks;
    std::vector<uint8_t> _rank_small_blocks;
    uint64_t _buf = 0;
    rsdic_uint _offset = 0;
    rsdic_uint _bit_num = 0;
    rsdic_uint _one_num = 0;
    rsdic_uint _prev_one_num = 0;
    rsdic_uint _zero_num = 0;
};

} /* namespace rsdic */

#endif // RSDIC_RSDIC_BUILDER_HPP_
