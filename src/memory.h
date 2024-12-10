#ifndef MEMORY_H_
#define MEMORY_H_

#define MAIN_MEMORY_SIZE_BYTES 1048576

typedef struct Memory {
    char data[MAIN_MEMORY_SIZE_BYTES];
} Memory;

Memory init_memory();

#endif
