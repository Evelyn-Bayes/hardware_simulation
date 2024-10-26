#include <stdio.h>
#include <stdlib.h>

// Simulates a 1mb HDD

void hdd_write(int offset, int bytes, char *data) {
    if (offset < 0) {
        printf("Invalid offset!\n");
        exit(1);
    }
    if (offset + bytes > 1048576) {
        printf("Write extends beyond end of disk!\n");
        exit(1);
    }

    FILE *file = fopen("/Users/evelynbayes/Desktop/disk", "w");
    fseek(file, offset, 0);
    fwrite(data, 1, bytes, file);
    fclose(file);
}

char* hdd_read(int offset, int bytes) {
    if (offset < 0) {
        printf("Invalid offset %d may not be negative!\n", offset);
        exit(1);
    }
    if (offset + bytes > 1048576) {
        printf("Read extends beyond end of disk!\n");
        exit(1);
    }

    char *data = malloc(bytes);

    FILE *file = fopen("/Users/evelynbayes/Desktop/disk", "r");
    fseek(file, offset, 0);
    fread(data, 1, bytes, file);
    fclose(file);

    return data;
}
