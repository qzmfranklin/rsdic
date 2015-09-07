#include "DawgBuilder.h"
#include <gtest/gtest.h>
#include <stdio.h>

TEST(DawgBuilder, input) {
    const char fname[] = "wordlist_sorted_356.txt.reversed";
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        fprintf(stderr,"Cannot open file: %s\n", fname);
        exit(1);
    }

    DawgBuilder g;
    g.make_root();

    std::vector<std::string> true_wordlist;

    {
        char *buf = NULL;
        size_t len;
        ssize_t bytesread;

        while (1) {
            bytesread = getline(&buf, &len, fp);
            if (bytesread == -1)
                break;
            buf[bytesread - 1] = 0; // remove the trailing '\n'
            //printf("<%s>\n", buf);
            true_wordlist.push_back(std::string(buf));
            g.add_word(std::string(buf));
        }
    }

    //g.build();
}
