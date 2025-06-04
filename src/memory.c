/*********************************************************************************************************************
 * Simulates a DRAM device                                                                                           *
 *********************************************************************************************************************/

#include "memory.h"
#include <stdint.h>

Memory init_memory() {
    Memory memory;
    for (uint32_t i = 0; i < MEMORY_SIZE_BYTES; i++) {
        memory.data[i] = 0;
    }
    return memory;
}
