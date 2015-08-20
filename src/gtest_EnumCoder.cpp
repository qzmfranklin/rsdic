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

#include <gtest/gtest.h>
#include "EnumCoder.h"

using namespace rsdic;

uint64_t pop_count(uint64_t x)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < 64; ++i) {
        if ((x >> i) & 1LLU) ++count;
    }
    return count;
}

TEST(EnumCoder, small)
{
    uint64_t code = EnumCoder::encode(0, pop_count(0));
    EXPECT_EQ(0, EnumCoder::decode(0, code));
}

TEST(EnumCoder, random)
{
    for (uint64_t i = 0; i < 10000; ++i) {
        uint64_t x = rand();
        uint64_t rank_sb = pop_count(x);
        uint64_t code = EnumCoder::encode(x, rank_sb);
        EXPECT_EQ(x, EnumCoder::decode(code, rank_sb));
    }
}
