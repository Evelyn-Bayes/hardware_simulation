/*
 * Simulates a CPU using a 32 bit word size
 *
 * Instruction set and list of registers are outlined in the ARCHITECTURE_SPECIFICATIONS.txt file
 * CPU cache is currently not in use
*/

#include "bit_macros.h"
#include "cpu.h"
#include "memory.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CYCLES_PER_SECOND 10000

// L1 Cache
#define L1_CACHE_CYCLE_COST 10
#define L1_CACHE_BYTE_SIZE 65536

// L2 Cache
#define L2_CACHE_CYCLE_COST 100
#define L2_CACHE_BYTE_SIZE 262144

// TODO: Implement memory initialisation
CPU init_cpu() {
    CPU cpu;
    cpu.program_counter = 0;
    for (int i = 0; i < 8; i++) {
        cpu.registers[i] = 0;
    }
    return cpu;
}

char get_op_code(int word) {
    int op_code_bit_filter = BIT_MASK_4 | BIT_MASK_3 | BIT_MASK_2 | BIT_MASK_1;
    return word & op_code_bit_filter;
}

void execute_jmp_instruction(int word, CPU *cpu_ptr) {
    bool use_higher_order_bits_as_offset = (word & BIT_MASK_5) >> 4;
    int base_register = (word & (BIT_MASK_8 | BIT_MASK_7 | BIT_MASK_6)) >> 5;
    bool jmp_control = (word & BIT_MASK_9) >> 8;
    bool jmp_control_register = (word & (BIT_MASK_12 | BIT_MASK_11 | BIT_MASK_10)) >> 9;
    int offset = (word & 0b1111111111111111111000000000000) >> 12;

    if (!use_higher_order_bits_as_offset) {
        offset = cpu_ptr->registers[offset];
    }

    bool skip_jmp_operation = jmp_control && !cpu_ptr->registers[jmp_control_register];

    if (!skip_jmp_operation) {
        printf("Base Register %d\n", base_register);
        printf("Base Register Value %d\n", cpu_ptr->registers[base_register]);
        printf("Offset %d\n", offset);
        cpu_ptr->program_counter = cpu_ptr->registers[base_register] + offset;
    }
}

void execute_ld_instruction(int word, CPU *cpu_ptr, Memory *memory_ptr) {
    int byte_indicator_bitmask = BIT_MASK_5 | BIT_MASK_6;
    int use_offset_value_bitmask = BIT_MASK_7;
    int destination_register_bitmask = BIT_MASK_8 | BIT_MASK_9 | BIT_MASK_10;
    int base_register_bitmask = BIT_MASK_11 | BIT_MASK_12 | BIT_MASK_13;
    int offset_bitmask = BIT_MASK_14 | BIT_MASK_15 | BIT_MASK_16 | BIT_MASK_17 | BIT_MASK_18 | BIT_MASK_19 | BIT_MASK_20 |
                            BIT_MASK_21 | BIT_MASK_22 | BIT_MASK_23 | BIT_MASK_24 | BIT_MASK_25 | BIT_MASK_26 | BIT_MASK_27 |
                            BIT_MASK_28 | BIT_MASK_29 | BIT_MASK_30 | BIT_MASK_31 | BIT_MASK_32;

    int byte_indicator = (word & byte_indicator_bitmask) >> 4;
    bool use_offset_value = (word & use_offset_value_bitmask) >> 6;
    int destination_register = (word & destination_register_bitmask) >> 7;
    int base_register = (word & base_register_bitmask) >> 10;
    int offset = (word & offset_bitmask) >> 13;

    if (!use_offset_value) {
        offset = cpu_ptr->registers[offset];
    }

    if (byte_indicator == 0) {
        cpu_ptr->registers[destination_register] = memory_ptr->data[base_register + offset];
    } else if (byte_indicator == 1) {
        cpu_ptr->registers[destination_register] = ((int) memory_ptr->data[base_register + offset]) << 8 | memory_ptr->data[base_register + offset + 1];
    } else if (byte_indicator == 2) {
        cpu_ptr->registers[destination_register] = ((int) memory_ptr->data[base_register + offset]) << 24 | ((int) memory_ptr->data[base_register + offset] + 1) << 16 | ((int) memory_ptr->data[base_register + offset] + 2) << 8 | memory_ptr->data[base_register + offset + 3];
    }
}

void execute_instruction(int word, CPU *cpu_ptr, Memory *memory_ptr) {
    int op_code = get_op_code(word);
    switch (op_code) {
        case 0:
            execute_jmp_instruction(word, cpu_ptr);
            break;
        case 1 :
            execute_ld_instruction(word, cpu_ptr, memory_ptr);
            break;
    }
}
