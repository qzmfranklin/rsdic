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
#include "RsdicBuilder.h"

#define private public
#include "Rsdic.h"
#include "EnumCoder.h"


using namespace rsdic;

TEST(BitVec, combination)
{
    for (uint64_t i = 0; i <= 64; ++i) {
        EXPECT_EQ(1, EnumCoder::kCombinationTable64_[i][i]);
    }
    for (uint64_t i = 1; i <= 64; ++i) {
        for (uint64_t j = 1; j <= i; ++j) {
            EXPECT_EQ(EnumCoder::kCombinationTable64_[i][j],
                      EnumCoder::kCombinationTable64_[i-1][j-1] +
                      EnumCoder::kCombinationTable64_[i-1][j]);
        }
    }
}


TEST(Rsdic, small)
{
    RsdicBuilder bvb;
    const uint64_t n = 64;
    for (int i = 0; i < n; i++)
        bvb.push_back(1);

    Rsdic bv;
    bvb.build(bv);
    EXPECT_EQ(n, bv.size());
    EXPECT_EQ(n, bv.one_num());
    for (size_t i = 0; i < bv.size(); i++) {
        EXPECT_EQ(1, bv.get_bit(i));
        EXPECT_EQ(i + 1, bv.rank1(i));
        EXPECT_EQ(i, bv.select1(i + 1));
    }
}


TEST(Rsdic, trivial_zero)
{
    RsdicBuilder bvb;
    const uint64_t n = 10000;
    for (int i = 0; i < n; i++)
        bvb.push_back(0);

    Rsdic bv;
    bvb.build(bv);
    EXPECT_EQ(n, bv.size());
    EXPECT_EQ(0, bv.one_num());
    for (size_t i = 0; i < bv.size(); i++) {
        EXPECT_EQ(0, bv.get_bit(i));
        EXPECT_EQ(i + 1, bv.rank0(i));
        EXPECT_EQ(i, bv.select0(i + 1));
    }
}

TEST(Rsdic, trivial_one)
{
    RsdicBuilder bvb;
    const uint64_t n = 10000;
    for (int i = 0; i < n; i++)
        bvb.push_back(1);

    Rsdic bv;
    bvb.build(bv);
    EXPECT_EQ(n, bv.size());
    EXPECT_EQ(n, bv.one_num());
    for (size_t i = 0; i < bv.size(); i++) {
        EXPECT_EQ(1, bv.get_bit(i));
        EXPECT_EQ(i + 1, bv.rank1(i));
        EXPECT_EQ(i, bv.select1(i + 1));
    }
}


TEST(Rsdic, random)
{
    RsdicBuilder bvb;
    std::vector<int> B;
    const uint64_t n = 100000;
    for (int i = 0; i < n; ++i) {
        int b = rand() % 2;
        bvb.push_back(b);
        B.push_back(b);
    }

    Rsdic bv;
    bvb.build(bv);
    EXPECT_EQ(n, bv.size());
    int sum = 0;
    for (size_t i = 0; i < bv.size(); ++i) {
        EXPECT_EQ(B[i]  , bv.get_bit(i)) << " i=" << i;
        bool bit;
        uint64_t rank;
        bv.get_bit_and_rank1(i, &bit, &rank);
        EXPECT_EQ(B[i], bit);
        if (B[i]) {
            EXPECT_EQ(sum + 1, bv.rank1(i));
            EXPECT_EQ(sum + 1, rank);
            EXPECT_EQ(i, bv.select1(sum + 1));
        } else {
            EXPECT_EQ(i - sum + 1, bv.rank0(i));
            EXPECT_EQ(i - sum, rank);
            EXPECT_EQ(i, bv.select0(i - sum + 1));
        }

        sum += B[i];
    }

    std::ostringstream os;
    bv.save(os);
    std::istringstream is(os.str());

    Rsdic bv_load;
    bv_load.load(is);

    EXPECT_EQ(bv, bv_load);
}

TEST(Rsdic, large)
{
    RsdicBuilder rsdb;
    const uint64_t n = 16llu * 1024llu * 1024llu; // 4MB
    std::vector<uint64_t> poses;
    for (uint64_t i = 0; i < n; i++) {
        float r = (float)rand() / RAND_MAX;
        if (r < 0.5) {
            rsdb.push_back(1);
            poses.push_back(i);
        } else rsdb.push_back(0);
    }

    Rsdic bv;
    rsdb.build(bv);
    uint64_t one_num = bv.one_num();
    for (uint64_t i = 0; i < one_num; i++)
        EXPECT_EQ(poses[i], bv.select1(i + 1));
}
