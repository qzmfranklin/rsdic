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

#ifndef RSDIC_RSDIC_HPP_
#define RSDIC_RSDIC_HPP_

#include "Type.h"

#include <vector>
#include <string>
#include <iostream>

namespace rsdic{

class Rsdic{
public:
  void clear();
  bool get_bit(const size_t pos) const;

  // number of 0/1's in [0, pos - 1]
  uint64_t rank0(const size_t pos) const;
  uint64_t rank1(const size_t pos) const;

  // position of (ind + 1)-th 0/1
  uint64_t select0(const size_t ind) const;
  uint64_t select1(const size_t ind) const;

  std::pair<uint64_t, uint64_t> get_bit_and_rank(size_t pos) const;
  bool operator == (const Rsdic& bv) const;

  // Allows us to load the tree quickly
  void load(const void*, const size_t);

  // Deprecated, will be removed
  void save(std::ostream &os) const;
  void load(std::istream &is);

  friend class RsdicBuilder;

private:
  uint64_t get_usage_bytes() const;
  uint64_t num() const{ return _num; }
  uint64_t one_num() const{ return _one_num; }
  uint64_t zero_num() const{ return _num - _one_num; }
  uint64_t _num = 0;
  uint64_t _one_num = 0;
  std::vector<uint64_t> _bits;
  std::vector<rsdic_uint> _pointer_blocks;
  std::vector<rsdic_uint> _rank_blocks;
  std::vector<rsdic_uint> _select_one_inds;
  std::vector<rsdic_uint> _select_zero_inds;
  std::vector<uint8_t> _rank_small_blocks;
};

} // rsdic

#endif // RSDIC_BITVEC_HPP_
