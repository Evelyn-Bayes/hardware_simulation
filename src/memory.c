/**********************************************************************************************************************
* Simulates a DRAM device with a 1MB capacity                                                                         *
**********************************************************************************************************************/

#include <stdint.h>
#include "memory.h"

const short MAIN_MEMORY_COST_CYCLES = 1000;

Memory init_memory() {
    Memory memory;
    for (uint32_t i = 0; i < MAIN_MEMORY_SIZE_BYTES; i++) {
        memory.data[i] = 0;
    }
    return memory;
}
