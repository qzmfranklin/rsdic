#include "WordTree.h"
#include "src/os/path.h"
#include "src/os/os.h"

#include <gtest/gtest.h>

#include <fstream>
#include <stdio.h>

TEST(WordTree, test) {
    const std::string this_dir = os::path::realpath(os::path::dirname(__FILE__));

    char *buf = NULL;
    size_t len = 0;
    { // Load image into buffer
        const std::string image_fname = os::path::normpath(os::path::join({ this_dir, "../builder/OUTPUT/dict.dat" }));
        //printf("%s\n", image_fname.c_str());

        ASSERT_TRUE(os::path::exists(image_fname));
        len = os::path::getsize(image_fname);
        //printf("filesize = %zu\n", len);

        FILE *fp = fopen(image_fname.c_str(), "r");
        if (!fp) {
            fprintf(stderr, "Cannot open file: %s", image_fname.c_str());
            exit(1);
        }
        buf = (char*)malloc(len + 1);
        assert(buf);
        const ssize_t bytesread = fread(buf, 1, len, fp);
        if (bytesread != len  ||  bytesread == -1) {
            printf("Cannot read all %zu bytes (read %zd)\n", len, bytesread);
            perror("WTF");
            exit(1);
        }
        fclose(fp);
    }

    // Create the WordTree from the loaded buffer
    dict::WordTree g(buf, len);

    {
        // Check that the wordlist from the loaded binary image is exactly the
        // same as the wordlist that was used to build the binary in the first
        // place.

        std::vector<std::string> wordlist0;
        { // Load original wordlist to wordlist0
            const std::string fname = os::path::normpath(os::path::join({ this_dir, "../test_data/highlights/en-US.txt" }));
            //fprintf(stderr,"%s\n", fname.c_str());
            std::ifstream is(fname);
            std::string tmp;
            while(std::getline(is, tmp))
                wordlist0.push_back(tmp);
            std::sort(wordlist0.begin(), wordlist0.end());
        }

        //g.dump_louds_debug();
        const  std::vector<std::string> wordlist1 = g.get_all_words();
        ASSERT_EQ(wordlist0.size(), wordlist1.size());
        for (size_t i = 0; i < wordlist0.size(); i++)
            ASSERT_EQ(wordlist0[i], wordlist1[i]);
    }

    { // Release buffer
        free(buf);
    }
}
