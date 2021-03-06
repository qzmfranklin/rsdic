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

#ifndef RSDIC_ENUM_CODER_HPP_
#define RSDIC_ENUM_CODER_HPP_

#include <stdint.h>
#include "Const.h"

namespace rsdic{

class EnumCoder{
public:
  static uint64_t encode(uint64_t val, uint64_t rank_sb);
  static uint64_t decode(uint64_t code, uint64_t rank_sb);
  static bool get_bit(uint64_t code, uint64_t rank_sb, uint64_t pos);
  static uint64_t rank(uint64_t code, uint64_t rank_sb, uint64_t pos);

  static uint64_t len(uint64_t rank_sb){
    return kEnumCodeLength_[rank_sb];
  }

  static uint64_t select0(uint64_t code, uint64_t rank_sb, uint64_t num);
  static uint64_t select1(uint64_t code, uint64_t rank_sb, uint64_t num);

private:
  static uint64_t pop_count(uint64_t code);
  static uint64_t select_raw(uint64_t code, uint64_t num);
  static const uint8_t kPopCount_[256];
  static const uint64_t kCombinationTable64_[65][65];
  static const uint8_t kEnumCodeLength_[65];
};

}

#endif // RSDIC_ENUM_CODER_HPP_
