#include "DawgBuilder.h"
#include <gtest/gtest.h>
#include <stdio.h>

TEST(DawgBuilder, input) {
    const char fname[] = "wordlist.txt";
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        fprintf(stderr,"Cannot open file: %s\n", fname);
        exit(1);
    }

    DawgBuilder g;
    std::vector<std::string> wordlist0;
    {
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

    {
        const std::vector<std::string> wordlist1 = g.export_all_words_debug();
        EXPECT_EQ(wordlist0.size(), wordlist1.size());
        for(size_t i = 0; i < wordlist1.size(); i++)
            EXPECT_EQ(wordlist0[i], wordlist1[i]);
    }

    //g.build();
}
