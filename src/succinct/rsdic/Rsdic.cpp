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

#include <sstream>

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

// TODO: This function is not being unit tested yet.
void Rsdic::get_bit_and_rank0(const uint64_t pos, bool *bit, uint64_t *rank0) const
{
    uint64_t rank1;
    this->get_bit_and_rank1(pos, bit, &rank1);
    *rank0 = pos + 1 - rank1;
    //fprintf(stderr,"bit = %c, pos = %llu, rank1 = %llu, rank0 = %llu\n", *bit ? '1' : '0', pos, rank1, *rank0);
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
    *rank1 = ret_bit ? rank + 1 : rank;
}

void Rsdic::get_bit_and_rank(const uint64_t pos, bool *bit, uint64_t *rank1) const
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
  const uint64_t size = vs.size();
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

template <typename T>
uint64_t _load_cstyle(const uint8_t *buf, std::vector<T>& vs) {
    uint64_t bytesread = 0;
    uint64_t size;
    memcpy(&size, buf + bytesread, sizeof(size));
    bytesread += sizeof(size);
    vs.resize(size);
    memcpy(&vs[0], buf + bytesread, sizeof(vs[0]) * size);
    bytesread += sizeof(vs[0]) * size;
    return bytesread;
}

} // anonymous namespace

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

// The save operation does not have to be super fast. Therefore this is just a
// thin wrapper on the C++ stream save().
size_t Rsdic::save_cstyle(void *buf) const
{
  std::stringstream ss;
  this->save(ss);
  const std::string tmp = ss.str();
  memcpy(buf, tmp.data(), tmp.length());
  return tmp.length();
}

size_t Rsdic::load_cstyle(const void *buf)
{
    const uint8_t *ptr = reinterpret_cast<const uint8_t*>(buf);

    memcpy(&_num, ptr, sizeof(_num));
    ptr += sizeof(_num);

    memcpy(&_one_num, ptr, sizeof(_one_num));
    ptr += sizeof(_one_num);

    ptr += _load_cstyle(ptr, _bits);
    ptr += _load_cstyle(ptr, _pointer_blocks);
    ptr += _load_cstyle(ptr, _rank_blocks);
    ptr += _load_cstyle(ptr, _select_one_inds);
    ptr += _load_cstyle(ptr, _select_zero_inds);
    ptr += _load_cstyle(ptr, _rank_small_blocks);

    return ptr - (const uint8_t*)buf;
}

namespace {

template <typename T>
size_t _count_bytes(const std::vector<T> &v) {
    assert(!v.empty());
    const uint64_t size = v.size();
    return sizeof(size) + sizeof(v[0]) * size;
}

}

size_t Rsdic::binary_size() const
{
    return sizeof(_num) + sizeof(_one_num)
        + _count_bytes(_bits)
        + _count_bytes(_pointer_blocks)
        + _count_bytes(_rank_blocks)
        + _count_bytes(_select_one_inds)
        + _count_bytes(_select_zero_inds)
        + _count_bytes(_rank_small_blocks)
        ;
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

void Rsdic::print() const
{
    fprintf(stdout,"    size        = %llu\n", _num);
    fprintf(stdout,"    one_num     = %llu\n", _one_num);
    fprintf(stdout,"    zero_num    = %llu\n", this->zero_num());
    fprintf(stdout,"    one_ratio   = %lf\n", static_cast<double>(_one_num) / _num);

    fprintf(stdout,"    bits        = %lu\n" , _bits.size() * sizeof(_bits[0]) );
    fprintf(stdout,"    ptr_blk     = %lu\n" , _pointer_blocks.size() * sizeof(_pointer_blocks[0]) );
    fprintf(stdout,"    rank_blk    = %lu\n" , _rank_blocks.size() * sizeof(_rank_blocks[0]) );
    fprintf(stdout,"    sel1ind     = %lu\n" , _select_one_inds.size() * sizeof(_select_one_inds[0]) );
    fprintf(stdout,"    sel0ind     = %lu\n" , _select_zero_inds.size() * sizeof(_select_zero_inds[0]) );
    fprintf(stdout," rank_small_blk = %lu\n", _rank_small_blocks.size() * sizeof(_rank_small_blocks[0]) );

}

}
