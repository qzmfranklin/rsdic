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

#include <string>
#include <gtest/gtest.h>
#include "Const.h"
#include "EnumCoder.h"

#define private public
#include "Rsdic.h"
#include "RsdicBuilder.h"


TEST(RsdicBuilder, trivial)
{
    rsdic::RsdicBuilder bvb;
    rsdic::Rsdic bv;
    bvb.build(bv);

    EXPECT_EQ(0, bv.num());
    EXPECT_EQ(0, bv.one_num());
}

TEST(RsdicBuilder, small)
{
    rsdic::RsdicBuilder bvb;
    rsdic::Rsdic bv;

    bvb.push_back(0);
    bvb.push_back(1);
    bvb.push_back(0);
    bvb.build(bv);

    EXPECT_EQ(3, bv.num());
    EXPECT_EQ(1, bv.one_num());
}

TEST(RsdicBuilder, EnumCodeSmall)
{
    uint64_t code = rsdic::EnumCoder::encode(25, 3);
    EXPECT_EQ(41540, code);
    EXPECT_EQ(16, rsdic::EnumCoder::len(3));
}

TEST(RsdicBuilder, EnumCode)
{
    std::string bits = "1110000011010011110011110110011001011111101011010111000000010010";
    uint64_t val = 0;
    uint64_t one_num = 0;
    EXPECT_EQ(64, bits.size());
    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits[i] == '1') {
            val |= 1LLU << i;
            ++ one_num;
        }
    }
    uint64_t code = rsdic::EnumCoder::encode(val, one_num);
    uint64_t decoded_bits = rsdic::EnumCoder::decode(code, one_num);
    EXPECT_EQ(val, decoded_bits);
}
