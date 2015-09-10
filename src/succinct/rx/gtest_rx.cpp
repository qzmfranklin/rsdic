#include "rx.h"
#include <gtest/gtest.h>

TEST(rx, rbx) {
    unsigned char *data = nullptr;
    int size = 0;
    std::vector<std::string> wordlist0;
    { // Build rbx image
        struct rbx_builder *builder = rbx_builder_create();
        /*
         * Sets length encoding paramenter. At default and this example, it is
         * usually set as 4 + 1n, where n is the number of bytes of the blob
         * being encoded
         */
        rbx_builder_set_length_coding(builder, 1, 1);

        { // Add blobs from the word list
            const char *fname = "wordlist.txt";
            FILE *fp = fopen(fname, "r");
            if (!fp) {
                fprintf(stderr,"Cannot open file: %s\n", fname);
                exit(1);
            }
            char *buf = NULL;
            size_t len;
            ssize_t bytesread;
            while (1) {
                bytesread = getline(&buf, &len, fp);
                if (bytesread == -1)
                    break;
                buf[bytesread - 1] = 0; // remove the trailing '\n'
                //printf("<%s>\n", buf);
                rbx_builder_push(builder, buf, bytesread - 1);
                wordlist0.push_back(std::string(buf));
            }
        }

        rbx_builder_build(builder);

        const unsigned char *buf = rbx_builder_get_image(builder);
        size = rbx_builder_get_size(builder);
        data = (unsigned char*) malloc(size);
        assert(data);
        memcpy(data, buf, size);

        printf("data = %p, size = %d\n", data, size);

        rbx_builder_release(builder);
    }

    { // Check that data retrieved from the blobs are the same as the original data
        struct rbx *rbx = rbx_open(data);

        const size_t num = wordlist0.size();
        for(int i = 0; i < num; i++) {
            const char *buf;
            int len;
            buf = (const char*)rbx_get(rbx, i, &len);
            //printf("[%5d] %s (%d)\n", i, buf, len);
            ASSERT_EQ(wordlist0[i], std::string(buf, len));
        }

        rbx_close(rbx);
    }
}
