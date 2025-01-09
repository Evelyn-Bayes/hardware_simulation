#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <inttypes.h>

#define MAIN_MEMORY_SIZE_BYTES 1048576

typedef struct Memory {
    uint8_t data[MAIN_MEMORY_SIZE_BYTES];
} Memory;

Memory init_memory();

#endif
