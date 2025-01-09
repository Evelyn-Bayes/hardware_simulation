#ifndef _CPU_H_
#define _CPU_H_

#include <inttypes.h>
#include "memory.h"

typedef struct CPU {
    uint32_t program_counter;

    /* General Purpose Registers */
    uint32_t registers[8];
} CPU;

CPU init_cpu();

void execute_instruction(uint32_t word, CPU *cpu, Memory *memory);

#endif
