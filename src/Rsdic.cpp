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

#include <cassert>
#include "Const.h"
#include "Util.h"
#include "EnumCoder.h"
#include "Rsdic.h"

using namespace std;

namespace rsdic
{

Rsdic::Rsdic() : _num(0), _one_num(0)
{
}

Rsdic::~Rsdic()
{
}

void Rsdic::clear()
{
    _bits.clear();
    _pointer_blocks.clear();
    _rank_blocks.clear();
    _select_one_inds.clear();
    _select_zero_inds.clear();
    _rank_small_blocks.clear();
    _num = 0;
    _one_num = 0;
}

bool Rsdic::get_bit(uint64_t pos) const
{
    uint64_t lblock = pos / kLargeBlockSize;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t sblock = pos / kSmallBlockSize;
    for (uint64_t i = lblock * kSmallBlockPerLargeBlock; i < sblock; ++i) {
        pointer += EnumCoder::len(_rank_small_blocks[i]);
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    return EnumCoder::get_bit(code, rank_sb, pos % kSmallBlockSize);
}

uint64_t Rsdic::rank(uint64_t pos, bool bit) const
{
    uint64_t lblock = pos / kLargeBlockSize;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t sblock = pos / kSmallBlockSize;
    uint64_t rank = _rank_blocks[lblock];
    for (uint64_t i = lblock * kSmallBlockPerLargeBlock; i < sblock; ++i) {
        uint64_t rank_sb = _rank_small_blocks[i];
        rank += rank_sb;
        pointer += EnumCoder::len(rank_sb);
    }
    if (pos % kSmallBlockSize == 0) {
        return Util::get_num(bit, rank, pos);
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    rank += EnumCoder::rank(code, rank_sb, pos % kSmallBlockSize);
    return Util::get_num(bit, rank, pos);
}

pair<uint64_t, uint64_t> Rsdic::get_bit_and_rank(uint64_t pos) const
{
    uint64_t lblock = pos / kLargeBlockSize;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t sblock = pos / kSmallBlockSize;
    uint64_t rank = _rank_blocks[lblock];
    for (uint64_t i = lblock * kSmallBlockPerLargeBlock; i < sblock; ++i) {
        uint64_t rank_sb = _rank_small_blocks[i];
        rank += rank_sb;
        pointer += EnumCoder::len(rank_sb);
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    rank += EnumCoder::rank(code, rank_sb, pos % kSmallBlockSize);
    uint64_t ret_bit = EnumCoder::get_bit(code, rank_sb, pos % kSmallBlockSize);
    return make_pair(ret_bit, Util::get_num(ret_bit, rank, pos));
}


uint64_t Rsdic::select1(uint64_t ind) const
{
    uint64_t select_ind = ind / kSelectBlockSize;
    uint64_t lblock = _select_one_inds[select_ind];
    for (; lblock < _rank_blocks.size(); ++lblock) {
        if (ind < _rank_blocks[lblock]) break;
    }
    --lblock;
    uint64_t sblock = lblock * kSmallBlockPerLargeBlock;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t remain = ind - _rank_blocks[lblock] + 1;

    for (; sblock < _rank_small_blocks.size(); ++sblock) {
        const uint64_t rank_sb = _rank_small_blocks[sblock];
        if (remain <= rank_sb) break;
        remain -= rank_sb;
        pointer += EnumCoder::len(rank_sb);
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    return sblock * kSmallBlockSize + EnumCoder::select1(code, rank_sb, remain);
}

uint64_t Rsdic::select0(uint64_t ind) const
{
    uint64_t select_ind = ind / kSelectBlockSize;
    uint64_t lblock = _select_zero_inds[select_ind];
    for (; lblock < _rank_blocks.size(); ++lblock) {
        if (lblock * kLargeBlockSize - _rank_blocks[lblock] > ind) break;
    }
    --lblock;

    uint64_t sblock = lblock * kSmallBlockPerLargeBlock;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t remain = ind - lblock * kLargeBlockSize + _rank_blocks[lblock] + 1;

    for (; sblock < _rank_small_blocks.size(); ++sblock) {
        const uint64_t rank_sb = kSmallBlockSize - _rank_small_blocks[sblock];
        if (remain <= rank_sb) break;
        remain -= rank_sb;
        pointer += EnumCoder::len(rank_sb);
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    return sblock * kSmallBlockSize + EnumCoder::select0(code, rank_sb, remain);
}

void Rsdic::save(ostream& os) const
{
    _save(os, _bits);
    _save(os, _pointer_blocks);
    _save(os, _rank_blocks);
    _save(os, _select_one_inds);
    _save(os, _select_zero_inds);
    _save(os, _rank_small_blocks);
    os.write((const char*)&_num, sizeof(_num));
    os.write((const char*)&_one_num, sizeof(_one_num));
}

void Rsdic::load(istream& is)
{
    _load(is, _bits);
    _load(is, _pointer_blocks);
    _load(is, _rank_blocks);
    _load(is, _select_one_inds);
    _load(is, _select_zero_inds);
    _load(is, _rank_small_blocks);
    is.read((char*)&_num, sizeof(_num));
    is.read((char*)&_one_num, sizeof(_one_num));
}

uint64_t Rsdic::get_usage_bytes() const
{
    /*
    cout << endl
         << "bits:" << _bits.size() * sizeof(_bits[0]) << endl
         << " ptb:" << _pointer_blocks.size() * sizeof(_pointer_blocks[0]) << endl
         << "  rb:" << _rank_blocks.size() * sizeof(_rank_blocks[0]) << endl
         << " soi:" << _select_one_inds.size() * sizeof(_select_one_inds[0]) << endl
         << " soz:" <<     _select_zero_inds.size() * sizeof(_select_zero_inds[0]) << endl
         << " rsb:" <<     _rank_small_blocks.size() * sizeof(_rank_small_blocks[0]) << endl;
    */
    return
        _bits.size() * sizeof(_bits[0]) +
        _pointer_blocks.size() * sizeof(_pointer_blocks[0]) +
        _rank_blocks.size() * sizeof(_rank_blocks[0]) +
        _select_one_inds.size() * sizeof(_select_one_inds[0]) +
        _select_zero_inds.size() * sizeof(_select_zero_inds[0]) +
        _rank_small_blocks.size() * sizeof(_rank_small_blocks[0]) +
        sizeof(_num) +
        sizeof(_one_num);
}

bool Rsdic::operator == (const Rsdic& bv) const
{
    return
        _bits == bv._bits &&
        _pointer_blocks == bv._pointer_blocks &&
        _rank_blocks == bv._rank_blocks &&
        _select_one_inds == bv._select_one_inds &&
        _select_zero_inds == bv._select_zero_inds &&
        _rank_small_blocks == bv._rank_small_blocks &&
        _num == bv._num &&
        _one_num == bv._one_num;
}


}
