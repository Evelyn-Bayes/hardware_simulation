#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <inttypes.h>

/* Memory sizes */
#define MEMORY_SIZE_BYTES 1048576 // 1MB

typedef struct Memory {
    uint8_t data[MEMORY_SIZE_BYTES]; 
} Memory;

Memory init_memory(void);

#endif
