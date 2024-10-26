extern "C" {
    #include "../src/hdd.h"
}
#include <gtest/gtest.h>

TEST(HDD, write_and_read_back) {
    char input[] = "Test String.";
    int len = sizeof(input);
    hdd_write(0, len, input);
    char* val = hdd_read(0, len);

    EXPECT_STREQ(val, "Test String.");
}
