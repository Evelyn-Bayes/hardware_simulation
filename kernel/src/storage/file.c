#include <stdio.h>

struct file {
    char[32] name;
    int file_number;
    int size_bytes;
    int file_system_offset;
    char* data;
}

int new(char[] name, int size_bytes) {
    return 0; // Default file_number until the file_system is implemented
}

char[] read(int file_number, int file_offset, int bytes) {
    
}

void write(int file_number, int file_offset, char[] data) {

}

void delete {

}
