/**********************************************************************************************************************
* Simulates a 100kb disk in a readable but... very sub-optimal way                                                    *
*                                                                                                                     *
* Abstraction Characteristics:                                                                                        *
*  - Bits are stored as a ZERO or ONE value rather then using changes of magnetic polarisation                          *
*                                                                                                                     *
* Storage Characteristics:                                                                                            *
*  - No sector preamble is included                                                                                   *
*  - No sector CRC is included                                                                                        *
*  - All cylinders contain the same number of sectors                                                                 *
*                                                                                                                     *
* Read / Write Characteristics:                                                                                       *
*  - Platters are attached to the same spindle and thus rotate together                                               *
*  - A read / write cannot be performed during a cylinder move as we would be unable to accurately interpret the data *
*  - A read / write must be performed over entire sectors                                                             *
*                                                                                                                     *
**********************************************************************************************************************/

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_HDD_FILE_NAME "/tmp/HDD"

/* HDD Space */
#define PLATTERS_PER_HDD 2
#define CYLINDERS_PER_PLATTER 10
#define SECTORS_PER_CYLINDER 10
#define BITS_PER_SECTOR 4096
#define MAX_ADDRESSABLE_BYTE INT32_C(PLATTERS_PER_HDD * CYLINDERS_PER_PLATTER * SECTORS_PER_CYLINDER * BITS_PER_SECTOR / 8)

/* Per Sector Header Formatting */
#define SECTOR_HEADER_TEMPLATE "Platter - %03d | Cylinder - %03d | Sector - %03d\n"
#define SECTOR_HEADER_SIZE 46

/* Useful ASCII Char Codes */
#define NEW_LINE 10
#define ZERO 48
#define ONE 49

/* Read / Write Characteristics */
#define TIME_UNITS_PER_SECOND 10000
#define TIME_UNITS_PER_CYLINDER_MOVE 100
#define TIME_UNITS_PER_BIT_MOVE 1

typedef struct {
    char platter;
    char cylinder;
    char sector;
    short bit;
} read_write_head;

/* Initialize Read / Write Heads */
read_write_head read_write_heads[2] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

void write_sector_header(FILE* file, char platter, char cylinder, char sector) {
    char buffer[SECTOR_HEADER_SIZE + 1];
    sprintf(buffer, SECTOR_HEADER_TEMPLATE, platter, cylinder, sector);
    fwrite(buffer, 1, SECTOR_HEADER_SIZE, file);
}

uint32_t address_to_hdd_file_offset(uint32_t address) {
    uint32_t sector = address / BITS_PER_SECTOR;
    uint32_t bit = address - sector * BITS_PER_SECTOR;
    if (sector > 0) {
        return sector * (BITS_PER_SECTOR + 1) + (sector + 1) * SECTOR_HEADER_SIZE + bit;
    } else {
        return SECTOR_HEADER_SIZE + bit;
    }
}

void hdd_write(uint32_t address, uint32_t bytes, char* data) {
    if (address + bytes > MAX_ADDRESSABLE_BYTE) {
        printf("Invalid address %"PRIu32", write of %"PRIu32" extends beyond end of disk of size %"PRIu32"!\n", 
                address, bytes, MAX_ADDRESSABLE_BYTE);
        exit(1);
    }

    FILE *file = fopen(DEFAULT_HDD_FILE_NAME, "r+");
    uint32_t adjusted_address = address_to_hdd_file_offset(address);
    fseek(file, adjusted_address, 0);
    fwrite(data, 1, bytes, file);
    fclose(file);
}

void hdd_read(uint32_t address, uint32_t bytes, char* buffer) {
    if (address + bytes > MAX_ADDRESSABLE_BYTE) {
        printf("Read extends beyond end of disk!\n");
        exit(1);
    }

    FILE *file = fopen(DEFAULT_HDD_FILE_NAME, "r");
    uint32_t adjusted_address = address_to_hdd_file_offset(address);
    fseek(file, adjusted_address, 0);
    fread(buffer, 1, bytes, file);
    fclose(file);
}

void format_disk(char* hdd_file_name) {
    char formatted_sector[BITS_PER_SECTOR];
    for (short i = 0; i < BITS_PER_SECTOR; i++) {
        formatted_sector[i] = ZERO;
    }

    FILE *file = fopen(hdd_file_name, "w");
    for (char i = 0; i < PLATTERS_PER_HDD; i++) {
        for (char j = 0; j < CYLINDERS_PER_PLATTER; j++) {
            for (char k = 0; k < SECTORS_PER_CYLINDER; k++) {
                write_sector_header(file, i, j, k);
                fwrite(formatted_sector, 1, BITS_PER_SECTOR, file);
                fputc(NEW_LINE, file);
            }
        }
    }
    fclose(file);
}
