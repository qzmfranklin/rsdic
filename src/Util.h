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

#ifndef RSDIC_UTIL_HPP_
#define RSDIC_UTIL_HPP_

#include <vector>
#include <stdint.h>
#include "Const.h"

namespace rsdic{

class Util{
public:
  static uint64_t get_slice(const std::vector<uint64_t>& bits,
                           uint64_t pos, uint64_t len) {
    if (len == 0) return 0;
    uint64_t block = pos / kSmallBlockSize;
    uint64_t offset = pos % kSmallBlockSize;
    uint64_t ret = bits[block] >> offset;
    if (offset + len > kSmallBlockSize){
      ret |= (bits[block+1] << (kSmallBlockSize - offset));
    }
    if (len == 64) return ret;
    return ret & ((1LLU << len) - 1);
  }

  static void set_slice(std::vector<uint64_t>& bits,
                       uint64_t pos, uint64_t len, uint64_t val) {
    if (len == 0) return;
    uint64_t block = pos / kSmallBlockSize;
    uint64_t offset = pos % kSmallBlockSize;
    bits[block] |= val << offset;
    if (offset + len > kSmallBlockSize){
      bits[block+1] |= val >> (kSmallBlockSize - offset);
    }
  }

  static uint64_t floor(uint64_t num, uint64_t div){
    return (num + div - 1) / div;
  }

  static void write_2bytes(void *buf, const uint16_t num) {
      uint8_t *ptr = reinterpret_cast<uint8_t*>(buf);
      ptr[0] = (0xFF00u & num) >> 8;
      ptr[1] =  0x00FFu & num;
  }

  static void write_4bytes(void *buf, const uint32_t num) {
      uint8_t *ptr = reinterpret_cast<uint8_t*>(buf);
      ptr[0] = (0xFF000000u & num) >> 24;
      ptr[1] = (0x00FF0000u & num) >> 16;
      ptr[2] = (0x0000FF00u & num) >> 8;
      ptr[3] =  0x000000FFu & num;
  }

  static uint16_t read_2bytes(const void *buf) {
      const uint8_t *ptr = reinterpret_cast<const uint8_t*>(buf);
      const uint16_t upper = ptr[0];
      const uint16_t lower = ptr[1];
      return (upper << 8u) | lower;
  }

  static uint32_t read_4bytes(const void *buf) {
      const uint8_t *ptr = reinterpret_cast<const uint8_t*>(buf);
      const uint32_t tmp0 = ptr[0];
      const uint32_t tmp1 = ptr[1];
      const uint32_t tmp2 = ptr[2];
      const uint32_t tmp3 = ptr[3];
      return (tmp0 << 24u) | (tmp1 << 16u) | (tmp2 << 8u) | tmp3;
  }

};

}

#endif // RSDIC_UTIL_HPP_
