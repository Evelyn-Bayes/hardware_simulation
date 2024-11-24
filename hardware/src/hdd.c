/* 
 * Simulates a 100kb disk in a readable but... very sub-optimal way
 *
 * Abstraction Characteristics:
 * - Bits are stored a ZERO or ONE values rather then using changes of magnetic polarisation
 * - Reads and writes are performed on a single platter at a time
 *
 * Storage Characteristics:
 * - No sector preamble is included
 * - No sector CRC is included
 * - All cylinders contain the same number of sectors
 *
 * Read / Write Characteristics:
 * - Platters are attached to the same spindle and thus rotate together
 * - A read / write cannot be performed during a cylinder move as we would be unable to accurately interpret the data
 * - A read / write must be performed over entire sectors
*/

#include <stdio.h>
#include <stdlib.h>

const char DEFAULT_HDD_FILE_NAME[] = "/tmp/HDD";

// HDD Space
const char PLATTERS_PER_HDD = 2;
const char CYLINDERS_PER_PLATTER = 10;
const char SECTORS_PER_CYLINDER = 10;
const short BITS_PER_SECTOR = 4096;
const int MAX_ADDRESSABLE_BYTE = PLATTERS_PER_HDD * CYLINDERS_PER_PLATTER * SECTORS_PER_CYLINDER * BITS_PER_SECTOR / 8;

// Per Sector Header Formatting
const char SECTOR_HEADER_TEMPLATE[] = "Platter - %03d | Cylinder - %03d | Sector - %03d\n";
const char SECTOR_HEADER_SIZE = 46;

// Useful ASCII Char Codes
const char NEW_LINE = 10;
const char ZERO = 48;
const char ONE = 49;

// Read / Write Characteristics
const short TIME_UNITS_PER_SECOND = 10000;
const char TIME_UNITS_PER_CYLINDER_MOVE = 100;
const char TIME_UNITS_PER_BIT_MOVE = 1;

typedef struct {
    char platter;
    char cylinder;
    char sector;
    short bit;
} read_write_head;

// Initialize Read / Write Heads
read_write_head read_write_heads[2];

void write_sector_header(FILE* file, char platter, char cylinder, char sector) {
    char buffer[SECTOR_HEADER_SIZE + 1];
    sprintf(buffer, SECTOR_HEADER_TEMPLATE, platter, cylinder, sector);
    fwrite(buffer, 1, SECTOR_HEADER_SIZE, file);
}

int address_to_hdd_file_offset(int address) {
    int sector = address / BITS_PER_SECTOR;
    int bit = address - sector * BITS_PER_SECTOR;
    if (sector > 0) {
        return sector * (BITS_PER_SECTOR + 1) + (sector + 1) * SECTOR_HEADER_SIZE; + bit;
    } else {
        return SECTOR_HEADER_SIZE; + bit;
    }
}

void hdd_write(int address, int bytes, char* data) {
    if (address < 0) {
        printf("Invalid address %d cannot be negative!\n", address);
        exit(1);
    }
    if (address + bytes > MAX_ADDRESSABLE_BYTE) {
        printf("Invalid address %d, write of %d extends beyond end of disk of size %d!\n", address, bytes, MAX_ADDRESSABLE_BYTE);
        exit(1);
    }

    FILE *file = fopen(DEFAULT_HDD_FILE_NAME, "r+");
    int adjusted_address = address_to_hdd_file_offset(address);
    fseek(file, adjusted_address, 0);
    fwrite(data, 1, bytes, file);
    fclose(file);
}

void hdd_read(int address, int bytes, char* buffer) {
    if (address < 0) {
        printf("Invalid address %d cannot be negative!\n", address);
        exit(1);
    }
    if (address + bytes > 1048576) {
        printf("Read extends beyond end of disk!\n");
        exit(1);
    }

    FILE *file = fopen(DEFAULT_HDD_FILE_NAME, "r");
    int adjusted_address = address_to_hdd_file_offset(address);
    fseek(file, adjusted_address, 0);
    fread(buffer, 1, bytes, file);
    fclose(file);
}



void hdd_format_disk(char* hdd_file_name) {
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
