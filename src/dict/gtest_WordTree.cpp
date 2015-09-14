#include "WordTree.h"
#include "src/os/path.h"
#include "src/os/os.h"

#include <gtest/gtest.h>

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

    dict::WordTree g(buf, len);

    { // Release buffer
        free(buf);
    }
}
