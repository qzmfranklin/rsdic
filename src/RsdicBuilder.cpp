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

#include "Rsdic.h"
#include "Const.h"
#include "Util.h"
#include "EnumCoder.h"
#include "RsdicBuilder.h"
#include "BytesAligner.h"

#include <iostream>

namespace rsdic
{

void RsdicBuilder::clear()
{
    _bits.clear();
    _pointer_blocks.clear();
    _select_one_inds.clear();
    _select_zero_inds.clear();
    _rank_blocks.clear();
    _rank_small_blocks.clear();
    _buf = 0;
    _offset = 0;
    _bit_num = 0;
    _one_num = 0;
    _prev_one_num = 0;
    _zero_num = 0;

    _state = EMPTY;
}

void RsdicBuilder::push_back(bool bit)
{
    assert(_state == EMPTY);
    if (_bit_num % kSmallBlockSize == 0) {
        _write_block();
    }
    if (bit) {
        _buf |= (1LLU << (_bit_num % kSmallBlockSize));
        if ((_one_num % kSelectBlockSize) == 0) {
            _select_one_inds.push_back(_bit_num / kLargeBlockSize);
        }
        ++_one_num;
    } else {
        if ((_zero_num % kSelectBlockSize) == 0) {
            _select_zero_inds.push_back(_bit_num / kLargeBlockSize);
        }
        ++_zero_num;
    }
    ++_bit_num;
}

void RsdicBuilder::_write_block()
{
    if (_bit_num > 0) {
        uint64_t rank_sb = _one_num - _prev_one_num;
        _rank_small_blocks.push_back(rank_sb);
        _prev_one_num = _one_num;

        uint64_t len = EnumCoder::len(rank_sb);
        uint64_t code = 0;
        if (len == kSmallBlockSize) {
            code = _buf; // use raw
        } else {
            code = EnumCoder::encode(_buf, rank_sb);
        }
        uint64_t new_size =  Util::floor(_offset + len, kSmallBlockSize);
        if (new_size > _bits.size()) {
            _bits.push_back(0);
        }
        Util::set_slice(_bits, _offset, len, code);
        _buf = 0;
        _offset += len;
    }
    if ((_bit_num % kLargeBlockSize) == 0) {
        _rank_blocks.push_back(_one_num);
        _pointer_blocks.push_back(_offset);
    }
}

void RsdicBuilder::build(Rsdic& bv)
{
    assert(_state == EMPTY);
    if (_bit_num == 0)
        return;

    _write_block();
    assert(_state == READY);

    bv.clear();
    bv._num                 = _bit_num;
    bv._one_num             = _one_num;
    bv._bits                = _bits;
    bv._select_one_inds     = _select_one_inds;
    bv._select_zero_inds    = _select_zero_inds;
    bv._pointer_blocks      = _pointer_blocks;
    bv._rank_blocks         = _rank_blocks;
    bv._rank_small_blocks   = _rank_small_blocks;
}

void RsdicBuilder::build()
{
    assert(_state == EMPTY);
    _write_block();
    _state = READY;
}

} // rsdic
