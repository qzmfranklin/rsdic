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

#include "src/os/path.h"

#define private public
#include "Rsdic.h"
#include "EnumCoder.h"

#include <fstream>

TEST(BitVec, combination)
{
    for (uint64_t i = 0; i <= 64; ++i) {
        EXPECT_EQ(1, rsdic::EnumCoder::kCombinationTable64_[i][i]);
    }
    for (uint64_t i = 1; i <= 64; ++i) {
        for (uint64_t j = 1; j <= i; ++j) {
            EXPECT_EQ(rsdic::EnumCoder::kCombinationTable64_[i][j],
                      rsdic::EnumCoder::kCombinationTable64_[i-1][j-1] +
                      rsdic::EnumCoder::kCombinationTable64_[i-1][j]);
        }
    }
}

TEST(Rsdic, small)
{
    rsdic::RsdicBuilder g;
    const uint64_t n = 64;
    for (int i = 0; i < n; i++)
        g.push_back(1);

    const rsdic::Rsdic v = g.build();
    EXPECT_EQ(n, v.size());
    EXPECT_EQ(n, v.one_num());
    for (size_t i = 0; i < v.size(); i++) {
        EXPECT_EQ(1, v.get_bit(i));
        EXPECT_EQ(i + 1, v.rank1(i));
        EXPECT_EQ(i, v.select1(i + 1));
    }
}

TEST(Rsdic, trivial_zero)
{
    rsdic::RsdicBuilder g;
    const uint64_t n = 10000;
    for (int i = 0; i < n; i++)
        g.push_back(0);

    const rsdic::Rsdic v = g.build();
    EXPECT_EQ(n, v.size());
    EXPECT_EQ(0, v.one_num());
    for (size_t i = 0; i < v.size(); i++) {
        EXPECT_EQ(0, v.get_bit(i));
        EXPECT_EQ(i + 1, v.rank0(i));
        EXPECT_EQ(i, v.select0(i + 1));
    }
}

TEST(Rsdic, trivial_one)
{
    rsdic::RsdicBuilder g;
    const uint64_t n = 10000;
    for (int i = 0; i < n; i++)
        g.push_back(1);

    const rsdic::Rsdic v = g.build();
    EXPECT_EQ(n, v.size());
    EXPECT_EQ(n, v.one_num());
    for (size_t i = 0; i < v.size(); i++) {
        EXPECT_EQ(1, v.get_bit(i));
        EXPECT_EQ(i + 1, v.rank1(i));
        EXPECT_EQ(i, v.select1(i + 1));
    }
}


TEST(Rsdic, get_bit_and_rank)
{
    rsdic::RsdicBuilder g;
    std::vector<int> B;
    const uint64_t n = 100000;
    for (int i = 0; i < n; ++i) {
        int b = rand() % 2;
        g.push_back(b);
        B.push_back(b);
    }

    const rsdic::Rsdic v = g.build();
    EXPECT_EQ(n, v.size());
    int sum = 0;
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(B[i]  , v.get_bit(i)) << " i=" << i;
        bool bit;
        uint64_t rank;
        v.get_bit_and_rank(i, &bit, &rank);
        EXPECT_EQ(B[i], bit);
        if (B[i]) {
            EXPECT_EQ(sum + 1, v.rank1(i));
            EXPECT_EQ(sum + 1, rank);
            EXPECT_EQ(i, v.select1(sum + 1));
        } else {
            EXPECT_EQ(i - sum + 1, v.rank0(i));
            EXPECT_EQ(i - sum, rank);
            EXPECT_EQ(i, v.select0(i - sum + 1));
        }

        sum += B[i];
    }

    std::ostringstream os;
    v.save(os);
    std::istringstream is(os.str());

    rsdic::Rsdic v_load;
    v_load.load(is);

    EXPECT_EQ(v, v_load);
}

TEST(Rsdic, large)
{
    rsdic::RsdicBuilder g;
    const uint64_t n = 16llu * 1024llu * 1024llu; // 4MB
    std::vector<uint64_t> poses;
    for (uint64_t i = 0; i < n; i++) {
        float r = (float)rand() / RAND_MAX;
        if (r < 0.5) {
            g.push_back(1);
            poses.push_back(i);
        } else g.push_back(0);
    }

    const rsdic::Rsdic v = g.build();
    uint64_t one_num = v.one_num();
    for (uint64_t i = 0; i < one_num; i++)
        EXPECT_EQ(poses[i], v.select1(i + 1));
}

TEST(Rsdic, CStyleLoad)
{
    const char *fname = "/tmp/haha";

    std::vector<uint64_t> poses;

    { // build and save
        rsdic::RsdicBuilder g;
        const uint64_t n = 16llu * 1024llu * 1024llu; // 4MB
        for (uint64_t i = 0; i < n; i++) {
            float r = (float)rand() / RAND_MAX;
            if (r < 0.5) {
                g.push_back(1);
                poses.push_back(i);
            } else g.push_back(0);
        }
        const rsdic::Rsdic v = g.build();

        std::ofstream os(fname);
        v.save(os);
    }

    { // load with stream and test
        rsdic::Rsdic v;
        std::ifstream is(fname);
        // This is what I want to test!
        v.load(is);
        uint64_t one_num = v.one_num();
        for (uint64_t i = 0; i < one_num; i++)
            EXPECT_EQ(poses[i], v.select1(i + 1));
    }

    { // load with C-style functions and test
        FILE *fp = fopen(fname, "r");
        const size_t len = os::path::getsize(fname);
        char *buf = (char *)malloc(len);
        fread(buf, 1, len, fp);
        fclose(fp);

        rsdic::Rsdic v;
        // This is what I want to test!
        v.load_cstyle(buf);
        uint64_t one_num = v.one_num();
        for (uint64_t i = 0; i < one_num; i++)
            EXPECT_EQ(poses[i], v.select1(i + 1));
    }
}

TEST(Rsdic, InitFromString)
{
    // Following the LOUDS convention, a super root, S in prepended to the bit vector.
    //
    //                                     S
    //                                     |
    //                                     1
    //                                    /|\
    //                                   / | \
    //                                  /  |  \
    //                                 /   |   \
    //                                2    3    4
    //                               / \   |  /   \
    //                              5   6  7 8     9
    //                                  |    |    / \
    //                                  10   11  12  13
    //                                           |
    //                                           14
    //
    //  node index  |   S |    1 |   2 |  3 |   4 | 5 |  6 | 7 |  8 |   9 | 10 |11 | 12 |13 |14
    // -------------+-----+------+-----+----+-----+---+----+---+----+-----+----+---+----+---+---
    std::string str = "10 | 1110 | 110 | 10 | 110 | 0 | 10 | 0 | 10 | 110 | 0  | 0 | 10 | 0 | 0";
    // -------------+-----+------+-----+----+-----+---+----+---+----+-----+----+---+----+---+---
    //     bit      |  01 | 2345 | 678 | 90 | 123 | 4 | 56 | 7 | 89 | 012 | 3  | 4 | 56 | 7 | 8
    //   offset     |         0+         |           10+          |            20+
    //
    // Given the bit offset m in the bit vector v:
    //      v[m] = 1:
    //          child(m)  = select0(rank1(m)) + 1
    //          parent(m) = select1(rank0(m))
    //          next(m)   = select0(rank0(m) + 1) + 1
    //      v[m] = 0:
    //          no child node
    //          parent(m) = select1(rank0(m) - 1)
    //          next(m)   = select0(rank0(m)) + 1
    rsdic::RsdicBuilder g;
    g.add_string(str);
    const rsdic::Rsdic v = g.build();

    // #8, m = 18, 19
    {   // first_child = #11
        //      rank1(18)        = 11
        //      select0(11) + 1  = 24 -> #11
        const uint64_t m = 18;
        const uint64_t rank1 = v.rank1(m);
        const uint64_t select0 = v.select0(rank1);
        EXPECT_EQ(11, rank1);
        EXPECT_EQ(24, select0 + 1);
    }

    // shorthand: #9 = node 9
    // #9, m = 20, 21, 22
    {   // first_child = #12
        //     rank1(20)         = 12
        //     select0(12) + 1   = 25 -> #12 (first bit)
        const uint64_t m = 20;
        const uint64_t rank1 = v.rank1(m);
        const uint64_t select0 = v.select0(rank1);
        EXPECT_EQ(12, rank1);
        EXPECT_EQ(25, select0 + 1);
    }
    {   // second_child = #13
        //     rank1(21)         = 13
        //     select0(13) + 1   = 27 -> #13 (first bit)
        const uint64_t m = 21;
        const uint64_t rank1 = v.rank1(m);
        const uint64_t select0 = v.select0(rank1);
        EXPECT_EQ(13, rank1);
        EXPECT_EQ(27, select0 + 1);
    }
    {   // parent = #4
        //     rank0(20, 21)     = 9
        //     select1(9)        = 12 -> #4 (second child)
        const uint64_t m = 21;
        const uint64_t rank0 = v.rank0(m);
        const uint64_t select1 = v.select1(rank0);
        EXPECT_EQ(9, rank0);
        EXPECT_EQ(12, select1);
    }
    {   // parent = #4
        //     rank0(22) - 1     = 9
        //     select1(9)        = 12 -> #4 (second child)
        const uint64_t m = 22;
        const uint64_t rank0 = v.rank0(m);
        const uint64_t select1 = v.select1(rank0 - 1);
        EXPECT_EQ(9, rank0 - 1);
        EXPECT_EQ(12, select1);
    }
}

TEST(Rsdic, overflow)
{
    // This test should test overflowing select and rank.
    // Looks like sometimes it crashes but other times it just returns a wrong
    // number. This forces us to check the range before all rank/select
    // operations that are possibly out of range.
    std::string str = "10 | 1110 | 110 | 10 | 110 | 0 | 10 | 0 | 10 | 110 | 0  | 0 | 10 | 0 | 0";
    rsdic::RsdicBuilder g;
    g.add_string(str);
    const rsdic::Rsdic v = g.build();

    EXPECT_EQ(v.one_num(), v.rank1(v.size() - 1));
    EXPECT_EQ(v.zero_num(), v.rank0(v.size() - 1));
    v.print();
}
