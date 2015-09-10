#include "src/rsdic/Rsdic.h"
#include "src/rsdic/RsdicBuilder.h"
#include "src/rx/rx.h"
#include "Tree.h"
#include <gtest/gtest.h>

#include <fstream>
#include <stdio.h>

TEST(Tree, input) {
    const char fname[] = "test_data/small_ascii.txt";
    //const char fname[] = "test_data/large_ascii.txt";
    //const char fname[] = "test_data/large_utf8.txt";
    //const char fname[] = "test_data/full_english.txt";
    //const char fname[] = "test_data/huge_utf8.txt";
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        fprintf(stderr,"Cannot open file: %s\n", fname);
        exit(1);
    }

    Tree g;
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
    const char *ofname = "OUTPUT/dict.dat";
    std::ofstream os;
    os.open(ofname, std::ios_base::out);
    { // Build bit vector and rbx data
        //std::string tmp = g.export_ascii_debug();
        std::string data  = g.export_data();
        std::string louds = g.export_louds();
        //printf("data\n%s\n", data.c_str());
        printf("louds\n%s\n", louds.c_str());

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

            { // Add blobs from the word list
                std::stringstream ss(data);
                std::string line;
                size_t rbx_useful = 0;
                while ( std::getline(ss, line, '\n') ) {
                    //printf("%s\n", line.c_str());
                    int ch;
                    sscanf(line.data(), "%X", &ch);
                    char buf[2] = "\0";
                    buf[0] = (char)ch;
                    size_t len = 1;
                    if (line.find("EOW") != std::string::npos) {
                        buf[1] = 0x01;
                        len = 2;
                    }
                    { // Check: can reconstruct the original line
                        char tmp[256];
                        snprintf(tmp, 256, "%X%s", buf[0], buf[1] == 0x01 ? "\tEOW" : "");
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
