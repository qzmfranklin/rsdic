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
#include "Util.h"

uint64_t GetBinLen(uint64_t x)
{
    uint64_t len = 0;
    for (; x >> len; ++len) {}
    return len;
}

TEST(Util, Slice)
{
    std::vector<uint64_t> vals;
    uint64_t offset = 0;
    for (uint64_t i = 0; i < 10000; ++i) {
        vals.push_back(rand());
    }
    for (uint64_t i = 0; i < vals.size(); ++i) {
        offset += GetBinLen(vals[i]);
    }
    std::vector<uint64_t> bits(rsdic::Util::floor(offset, 64));

    offset = 0;
    for (uint64_t i = 0; i < vals.size(); ++i) {
        uint64_t len = GetBinLen(vals[i]);
        rsdic::Util::set_slice(bits, offset, len, vals[i]);
        offset += len;
    }

    offset = 0;
    for (uint64_t i = 0; i < vals.size(); ++i) {
        uint64_t len = GetBinLen(vals[i]);
        EXPECT_EQ(vals[i], rsdic::Util::get_slice(bits, offset, len)) << i;
        offset += len;
    }
}
