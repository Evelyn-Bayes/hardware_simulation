extern "C" {
    #include "../src/hdd.h"
}
#include <gtest/gtest.h>


TEST(HDD, write_and_read_back) {
    char hdd_file_name[] = "/tmp/HDD";
    format_disk(hdd_file_name);

    char input[] = "Test String.";
    char string_length = sizeof(input) - 1;
    hdd_write(0, string_length, input);

    char output[string_length];
    hdd_read(0, string_length, output);

    EXPECT_STREQ(output, "Test String.");
}

TEST(HDD, format_drive) {
    char hdd_file_name[] = "/tmp/Formatted_HDD";
    format_disk(hdd_file_name);
    EXPECT_EQ(true, true);
}

TEST(HHD, address_to_block_and_offset) {
    EXPECT_EQ(true, true);
}

TEST(HHD, block_to_platter_track_sector) {
    EXPECT_EQ(true, true);
}

TEST(HDD, bit_array_to_pos_neg_magnetism_array) {
    // Translate to
    EXPECT_EQ(true, true);
    // Translate from
    EXPECT_EQ(true, true);
}

TEST(HDD, write_to_file_format) {
    EXPECT_EQ(true, true);
}

TEST(HDD, read_from_file_format) {
    EXPECT_EQ(true, true);
}

TEST(HDD, format_disk) {
    EXPECT_EQ(true, true);
}
/* 
 * Todo:
 * - Core read write functions which take a series of bytes and stores them to disk
 *     This should follow the same format for all storage mediums
 * - Simulation of blocks sectors tracks and plates
 * - Simulates information within a block including the crc
 * - Simulates data to the positive and negative charges which make up a disk
 * - 
 *
 *
 * In steps;
 *  - Write takes a series of bytes
 *  - Translates address to plate / track / sector 
 *  - If write, performs read into buffer then writes appropriate
 *  - If write recreates full block to write along with crc
 *  - Translates to on disk storage positive and negative magnetism plus accompanying metadata
 */
