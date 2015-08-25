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

uint64_t Rsdic::rank0(const uint64_t pos) const
{
    const uint64_t pos2 = pos + 1;
    uint64_t lblock = pos2 / kLargeBlockSize;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t sblock = pos2 / kSmallBlockSize;
    uint64_t rank = _rank_blocks[lblock];
    for (uint64_t i = lblock * kSmallBlockPerLargeBlock; i < sblock; ++i) {
        uint64_t rank_sb = _rank_small_blocks[i];
        rank += rank_sb;
        pointer += EnumCoder::len(rank_sb);
    }
    if (pos2 % kSmallBlockSize == 0) {
        return pos2 - rank;
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    rank += EnumCoder::rank(code, rank_sb, pos2 % kSmallBlockSize);
    return pos2 - rank;
}

uint64_t Rsdic::rank1(const uint64_t pos) const
{
    const uint64_t pos2 = pos + 1;
    uint64_t lblock = pos2 / kLargeBlockSize;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t sblock = pos2 / kSmallBlockSize;
    uint64_t rank = _rank_blocks[lblock];
    for (uint64_t i = lblock * kSmallBlockPerLargeBlock; i < sblock; ++i) {
        uint64_t rank_sb = _rank_small_blocks[i];
        rank += rank_sb;
        pointer += EnumCoder::len(rank_sb);
    }
    if (pos2 % kSmallBlockSize == 0) {
        return rank;
    }
    uint64_t rank_sb = _rank_small_blocks[sblock];
    uint64_t code = Util::get_slice(_bits, pointer, EnumCoder::len(rank_sb));
    rank += EnumCoder::rank(code, rank_sb, pos2 % kSmallBlockSize);
    return rank;
}

void Rsdic::get_bit_and_rank0(const uint64_t pos, bool *bit, uint64_t *rank0) const
{
    this->get_bit_and_rank1(pos, bit, rank0);
    *rank0 = pos + 1 - *rank0;
}

void Rsdic::get_bit_and_rank1(const uint64_t pos, bool *bit, uint64_t *rank1) const
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

    *bit   = ret_bit;
    *rank1 = ret_bit ? rank + 1 : pos - rank;

    //return make_pair(ret_bit, ret_bit ? rank : pos - rank);
}


uint64_t Rsdic::select1(const uint64_t ind) const
{
    const uint64_t ind2 = ind - 1;
    uint64_t select_ind = ind2 / kSelectBlockSize;
    uint64_t lblock = _select_one_inds[select_ind];
    for (; lblock < _rank_blocks.size(); ++lblock) {
        if (ind2 < _rank_blocks[lblock]) break;
    }
    --lblock;
    uint64_t sblock = lblock * kSmallBlockPerLargeBlock;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t remain = ind2 - _rank_blocks[lblock] + 1;

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

uint64_t Rsdic::select0(const uint64_t ind) const
{
    const uint64_t ind2 = ind - 1;
    uint64_t select_ind = ind2 / kSelectBlockSize;
    uint64_t lblock = _select_zero_inds[select_ind];
    for (; lblock < _rank_blocks.size(); ++lblock) {
        if (lblock * kLargeBlockSize - _rank_blocks[lblock] > ind2) break;
    }
    --lblock;

    uint64_t sblock = lblock * kSmallBlockPerLargeBlock;
    uint64_t pointer = _pointer_blocks[lblock];
    uint64_t remain = ind2 - lblock * kLargeBlockSize + _rank_blocks[lblock] + 1;

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

namespace {
template <typename T>
void _save(std::ostream& os, const std::vector<T>& vs) {
  uint64_t size = vs.size();
  os.write((const char*)&size, sizeof(size));
  os.write((const char*)&vs[0], sizeof(vs[0]) * size);
}

template <typename T>
void _load(std::istream& is, std::vector<T>& vs) {
  uint64_t size = 0;
  is.read((char*)&size, sizeof(size));
  vs.resize(size);
  is.read((char*)&vs[0], sizeof(vs[0]) * size);
}

} // anonymous namespace

// Deprecated
void Rsdic::save(std::ostream& os) const
{
    os.write((const char*)&_num, sizeof(_num));
    os.write((const char*)&_one_num, sizeof(_one_num));
    _save(os, _bits);
    _save(os, _pointer_blocks);
    _save(os, _rank_blocks);
    _save(os, _select_one_inds);
    _save(os, _select_zero_inds);
    _save(os, _rank_small_blocks);
}

// Deprecated
void Rsdic::load(std::istream& is)
{
    is.read((char*)&_num, sizeof(_num));
    is.read((char*)&_one_num, sizeof(_one_num));
    _load(is, _bits);
    _load(is, _pointer_blocks);
    _load(is, _rank_blocks);
    _load(is, _select_one_inds);
    _load(is, _select_zero_inds);
    _load(is, _rank_small_blocks);
}

// Used for fast loading
// TODO: Make it cleaner
void Rsdic::load(const void *buf, const uint64_t len)
{
    const uint8_t *ptr = reinterpret_cast<const uint8_t*>(buf);

    memcpy(&_num, ptr, sizeof(_num));
    ptr += sizeof(_num);

    memcpy(&_one_num, ptr, sizeof(_one_num));
    ptr += sizeof(_one_num);

    uint64_t size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof(size);
    memcpy(_bits.data(), ptr, size * sizeof(_bits[0]));
    ptr += sizeof(_bits[0]) * size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof(size);
    memcpy(_pointer_blocks.data(), ptr, size * sizeof(_pointer_blocks[0]));
    ptr += sizeof(_pointer_blocks[0]) * size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof(size);
    memcpy(_rank_blocks.data(), ptr, size * sizeof(_rank_blocks[0]));
    ptr += sizeof(_rank_blocks[0]) * size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof(size);
    memcpy(_select_one_inds.data(), ptr, size * sizeof(_select_one_inds[0]));
    ptr += sizeof(_select_one_inds[0]) * size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof(size);
    memcpy(_select_zero_inds.data(), ptr, size * sizeof(_select_zero_inds[0]));
    ptr += sizeof(_select_zero_inds[0]) * size;

    memcpy(&size, ptr, sizeof(size));
    ptr += sizeof(size);
    memcpy(_rank_small_blocks.data(), ptr, size * sizeof(_rank_small_blocks[0]));
    ptr += sizeof(_rank_small_blocks[0]) * size;
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

// TODO: Is this function ever useful?
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
