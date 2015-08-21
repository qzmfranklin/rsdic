#include "BytesAligner.h"
#include <gtest/gtest.h>

TEST(BytesAligner, string_vector) {
    printf("This test uses human visual. More of an example usage than a test\n");

    BytesAligner g;

    std::basic_string<uint32_t> v0;
    std::vector<uint64_t> v1;
    std::vector<uint8_t> v2;
    std::string v3;
    std::string v4;
    v0.resize(32);
    v1.resize(32);
    v2.resize(9);
    v3.resize(53);
    v4.resize(9989);

    g.add("v0", v0);
    g.add("v1", v1);
    g.add("v2", v2);
    g.add("v3", v3);
    g.add("v4", v4);
    g.print(stdout);
}
