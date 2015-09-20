#include "WordTree.h"
#include "src/os/path.h"
#include "src/os/os.h"

#include <gtest/gtest.h>

#include <fstream>
#include <unordered_set>
#include <stdio.h>

TEST(WordTree, test) {
    const std::string this_dir = os::path::realpath(os::path::dirname(__FILE__));

    char *buf = NULL;
    size_t len;
    { // Load image into buffer
        const std::string image_fname = os::path::normpath(os::path::join({ this_dir, "../builder/OUTPUT/dict.dat" }));
        //printf("%s\n", image_fname.c_str());

        ASSERT_TRUE(os::path::exists(image_fname));
        //const size_t len = os::path::getsize(image_fname);
        len = os::path::getsize(image_fname);
        //printf("filesize = %zu\n", len);

        FILE *fp = fopen(image_fname.c_str(), "r");
        if (!fp) {
            fprintf(stderr, "Cannot open file: %s", image_fname.c_str());
            exit(1);
        }
        buf = (char*)malloc(len);
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
    const dict::WordTree g(buf, len);
    free(buf);

    //g.dump_louds_debug();

    const std::string source_wordlist_fname = os::path::normpath(os::path::join({ this_dir, "../test_data/highlights/en-US.txt" }));
    std::vector<std::string> wordlist0;
    { // Load original wordlist to wordlist0
        //fprintf(stderr,"%s\n", source_wordlist_fname.c_str());
        std::ifstream is(source_wordlist_fname);
        std::string tmp;
        while(std::getline(is, tmp))
            wordlist0.push_back(tmp);
        std::sort(wordlist0.begin(), wordlist0.end());
    }

    {
        // Check that the wordlist from the loaded binary image is exactly the
        // same as the wordlist that was used to build the binary in the first
        // place.

        //g.dump_louds_debug();
        const  std::vector<std::string> wordlist1 = g.get_all_words();
        ASSERT_EQ(wordlist0.size(), wordlist1.size());
        for (size_t i = 0; i < wordlist0.size(); i++)
            ASSERT_EQ(wordlist0[i], wordlist1[i]);
    }

    { // Find the index_t of certain words
        // Verify two things:
        //   1.  All words in the wordlist have non-zero index
        //   2.  No two words have the same indices
        std::unordered_set<dict::WordTree::index_t> set;
        for (const auto &w: wordlist0) {
            //printf("%s\t\t%u\n", w.c_str(), g.find(w));
            const dict::WordTree::index_t index = g.find(w);
            ASSERT_TRUE(index);
            if (set.find(index) != set.end())
                ASSERT_TRUE(false);
            else
                set.insert(index);
        }
    }

    //free(buf);
}
