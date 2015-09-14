#include "src/builder/tree/Tree.h"
#include "src/os/path.h"
#include "src/os/os.h"
#include "src/succinct/rsdic/Rsdic.h"
#include "src/succinct/rsdic/RsdicBuilder.h"
#include "src/succinct/rx/rx.h"
#include <gtest/gtest.h>

#include <fstream>
#include <stdio.h>

TEST(Tree, input) {
    const std::string this_dir = os::path::realpath(os::path::dirname(__FILE__));
    std::string fname = os::path::normpath(os::path::join({
                this_dir, "../test_data/highlights/el-GR.txt"
            }));
    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp) {
        fprintf(stderr,"Cannot open file: %s\n", fname.c_str());
        exit(1);
    }

    builder::Tree g;
    std::vector<std::string> wordlist0;
    { // Add words from the word list to the dawg
        g.make_root();

        char *buf = NULL;
        size_t len;
        ssize_t bytesread;

        while (1) {
            bytesread = getline(&buf, &len, fp);
            if (bytesread == -1)
                break;
            buf[bytesread - 1] = 0; // remove the trailing '\n'
            //printf("<%s>\n", buf);
            wordlist0.push_back(std::string(buf));
            g.add_word(std::string(buf));
        }

        std::sort(wordlist0.begin(), wordlist0.end());
    }


    { // Test that the dawg has and only has all orignal words
        // Caveat: wordlist0 and wordlist1 must be sorted to allow easy comparison
        const std::vector<std::string> wordlist1 = g.export_sorted_wordlist_debug();
        EXPECT_EQ(wordlist0.size(), wordlist1.size());
        for(size_t i = 0; i < wordlist1.size(); i++)
            EXPECT_EQ(wordlist0[i], wordlist1[i]);
    }


    rsdic::Rsdic v;
    struct rbx *rbx = nullptr;
    const std::string ofname = os::path::join({this_dir, "OUTPUT", "dict.dat"});
    os::mkdir(os::path::join({this_dir, "OUTPUT"}));
    std::ofstream os;
    os.open(ofname, std::ios_base::out);
    { // Build bit vector and rbx data
        //std::string tmp = g.export_ascii_debug();
        std::string data  = g.export_data();
        std::string louds = g.export_louds();
        //printf("data\n%s\n", data.c_str());
        //printf("louds\n%s\n", louds.c_str());

        { // Build bit vector
            rsdic::RsdicBuilder builder;
            builder.add_string(louds);
            v = builder.build();
            v.save(os);
            printf("bitvec size = %llu\n", v.get_usage_bytes());
        }

        { // Build rbx
            struct rbx_builder *builder = rbx_builder_create();

            {
                /*
                 * Coding parameters: a, b
                 *        a + b * n
                 * No idea what they really are. But trying a=1-4 for the best
                 * value, i.e., smallest image size
                 */
                const int a = 1;
                const int b = 1;
                rbx_builder_set_length_coding(builder, a, b);
            }

            uint32_t rbx_size = 0;
            { // Add blobs from the word list
                std::stringstream ss(data);
                std::string line;
                size_t rbx_useful = 0;
                while ( std::getline(ss, line, '\n') ) {
                    rbx_size++;
                    //printf("%s\n", line.c_str());
                    int ch;
                    sscanf(line.data(), "%X", &ch);
                    char buf[2] = "\0";
                    buf[0] = (uint8_t)ch;
                    if (line.find("END_OF_WORD") != std::string::npos)
                        buf[1] |= 0x01;
                    if (line.find("LAST_CHILD") != std::string::npos)
                        buf[1] |= 0x01 << 1;
                    const size_t len = buf[1] ? 2 : 1;
                    { // Check: can reconstruct the original line
                        char tmp[256];
                        snprintf(tmp, 256, "%02X%s%s"
                                , (uint8_t)buf[0]
                                , buf[1] & 0x01 ? "\tEND_OF_WORD" : ""
                                , buf[1] & 0x01 << 1 ? "\tLAST_CHILD" : ""
                                );
                        ASSERT_EQ(std::string(tmp), line);
                    }
                    //printf("%2zu %X %X\n", len, buf[0], buf[1]);
                    rbx_builder_push(builder, buf, len);
                    rbx_useful += len;
                }
                printf("rbx_useful  = %zu\n", rbx_useful);
            }

            {
                rbx_builder_build(builder);

                const unsigned char *buf = rbx_builder_get_image(builder);
                const int size = rbx_builder_get_size(builder);
                unsigned char *data = (unsigned char*) malloc(size);
                os.write((const char*)&rbx_size, sizeof(rbx_size));
                os.write((const char*)data, size);
                assert(data);
                memcpy(data, buf, size);
                rbx = rbx_open(data);

                printf("rbx size    = %d\n", size);
                printf("total size  = %llu\n", size + v.get_usage_bytes());

                rbx_builder_release(builder);
            }
        }
    }

    rbx_close(rbx);
}
