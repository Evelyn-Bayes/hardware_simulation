#ifndef CPU_H_
#define CPU_H_

#include "memory.h"

typedef struct CPU {
    int program_counter;

    // General Purpose Registers
    int registers[8];
} CPU;

CPU init_cpu();

void execute_instruction(int word, CPU *cpu, Memory *memory);

#endif
