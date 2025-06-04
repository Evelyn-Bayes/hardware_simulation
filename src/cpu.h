#ifndef _CPU_H_
#define _CPU_H_

#include "memory.h"
#include <inttypes.h>

typedef struct Cpu {
    uint32_t program_counter;

    /* General Purpose Registers */
    uint32_t registers[8];
} Cpu;

Cpu init_cpu();

void execute_instruction(uint32_t word, Cpu *cpu, Memory *memory);

#endif
