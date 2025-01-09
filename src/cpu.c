/**********************************************************************************************************************
* Simulates a CPU using a 32 bit word size                                                                            *
*                                                                                                                     *
* Instruction set and list of registers are outlined in the ARCHITECTURE_SPECIFICATIONS.txt file                      *
* CPU cache is currently not in use                                                                                   *
**********************************************************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bit_utils.h"
#include "cpu.h"
#include "memory.h"

#define CYCLES_PER_SECOND 10000

/* L1 Cache */
#define L1_CACHE_CYCLE_COST 10
#define L1_CACHE_BYTE_SIZE 65536

/* L2 Cache */
#define L2_CACHE_CYCLE_COST 100
#define L2_CACHE_BYTE_SIZE 262144

CPU init_cpu() {
    CPU cpu = {
        0, // program counter
        {0, 0, 0, 0, 0, 0, 0, 0} // registers
    };
    return cpu;
}

char get_op_code(uint32_t word) {
    uint32_t op_code_bitmask = BITMASK_4_TO_1;
    return word & op_code_bitmask;
}

void execute_jmp_instruction(uint32_t word, CPU *cpu_ptr) {
    uint32_t use_higher_order_bits_as_offset_bitmask = BITMASK_5;
    uint32_t base_register_bitmask = BITMASK_8 | BITMASK_7 | BITMASK_6;
    uint32_t jmp_control_bitmask = BITMASK_9;
    uint32_t jmp_control_register_bitmask = BITMASK_12 | BITMASK_11 | BITMASK_10;
    uint32_t offset_bitmask = BITMASK_32_TO_17 | BITMASK_16 | BITMASK_15 | BITMASK_14 | BITMASK_13;

    bool use_higher_order_bits_as_offset = (word & use_higher_order_bits_as_offset_bitmask) >> 4;
    uint32_t base_register = (word & (base_register_bitmask)) >> 5;
    bool jmp_control = (word & jmp_control_bitmask) >> 8;
    bool jmp_control_register = (word & jmp_control_register_bitmask) >> 9;
    uint32_t offset = (word & offset_bitmask) >> 12;

    if (!use_higher_order_bits_as_offset) {
        offset = cpu_ptr->registers[offset];
    }

    bool skip_jmp_operation = jmp_control && !cpu_ptr->registers[jmp_control_register];

    if (!skip_jmp_operation) {
        cpu_ptr->program_counter = cpu_ptr->registers[base_register] + offset;
    }
}

void execute_ld_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t byte_indicator_bitmask = BITMASK_6 | BITMASK_5;
    uint32_t use_offset_value_bitmask = BITMASK_7;
    uint32_t destination_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    uint32_t base_register_bitmask = BITMASK_13 | BITMASK_12 | BITMASK_11;
    uint32_t offset_bitmask = BITMASK_32_TO_17| BITMASK_16 | BITMASK_15 | BITMASK_14;

    uint32_t byte_indicator = (word & byte_indicator_bitmask) >> 4;
    bool use_offset_value = (word & use_offset_value_bitmask) >> 6;
    uint32_t destination_register = (word & destination_register_bitmask) >> 7;
    uint32_t base_register = (word & base_register_bitmask) >> 10;
    uint32_t offset = (word & offset_bitmask) >> 13;

    if (!use_offset_value) {
        offset = cpu_ptr->registers[offset];
    }

    if (byte_indicator == 0) {
        cpu_ptr->registers[destination_register] = memory_ptr->data[base_register + offset];
    } else if (byte_indicator == 1) {
        cpu_ptr->registers[destination_register] = 
            ((uint32_t) memory_ptr->data[base_register + offset]) << 8 | memory_ptr->data[base_register + offset + 1];
    } else if (byte_indicator == 2) {
        cpu_ptr->registers[destination_register] = 
            ((uint32_t) memory_ptr->data[base_register + offset]) << 24 | 
            ((uint32_t) memory_ptr->data[base_register + offset] + 1) << 16 | 
            ((uint32_t) memory_ptr->data[base_register + offset] + 2) << 8 | 
            memory_ptr->data[base_register + offset + 3];
    }
}

void execute_put_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t byte_indicator_bitmask = BITMASK_6 | BITMASK_5;
    uint32_t use_offset_value_bitmask = BITMASK_7;
    uint32_t source_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    uint32_t base_register_bitmask = BITMASK_13 | BITMASK_12 | BITMASK_11;
    uint32_t offset_bitmask = BITMASK_32_TO_17| BITMASK_16 | BITMASK_15 | BITMASK_14;

    uint32_t byte_indicator = (word & byte_indicator_bitmask) >> 4;
    bool use_offset_value = (word & use_offset_value_bitmask) >> 6;
    uint32_t source_register = (word & source_register_bitmask) >> 7;
    uint32_t base_register = (word & base_register_bitmask) >> 10;
    uint32_t offset = (word & offset_bitmask) >> 13;

    if (!use_offset_value) {
        offset = cpu_ptr->registers[offset];
    }

    //TODO: The use of BITMASK_8_TO_1 may be redundant here, review it.
    if (byte_indicator == 0) {
        memory_ptr->data[base_register + offset] = cpu_ptr->registers[source_register];
    } else if (byte_indicator == 1) {
        memory_ptr->data[base_register + offset] = cpu_ptr->registers[source_register] >> 8 & BITMASK_8_TO_1;
        memory_ptr->data[base_register + offset + 1] = cpu_ptr->registers[source_register] & BITMASK_8_TO_1;
    } else if (byte_indicator == 2) {
        memory_ptr->data[base_register + offset] = cpu_ptr->registers[source_register] >> 24 & BITMASK_8_TO_1;
        memory_ptr->data[base_register + offset + 1] = cpu_ptr->registers[source_register] >> 16 & BITMASK_8_TO_1;
        memory_ptr->data[base_register + offset + 2] = cpu_ptr->registers[source_register] >> 8 & BITMASK_8_TO_1;
        memory_ptr->data[base_register + offset + 3] = cpu_ptr->registers[source_register] & BITMASK_8_TO_1;
    }
}

void execute_set_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t destination_register_bitmask = BITMASK_7 | BITMASK_6 | BITMASK_5;
    uint32_t control_bitmask = BITMASK_8;
    uint32_t value_bitmask = BITMASK_32_TO_13 | BITMASK_12 | BITMASK_11 | BITMASK_10 | BITMASK_9;

    uint32_t destination_register = (word & destination_register_bitmask) >> 4;
    bool set_negative_value = (word & control_bitmask) >> 7;
    uint32_t value = (word & value_bitmask) >> 8;

    if (set_negative_value) {
        value = BITMASK_32_TO_29 | BITMASK_28 | BITMASK_27 | BITMASK_26 | BITMASK_25 | value;
    }
    cpu_ptr->registers[destination_register] = value;
}

void execute_setu_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t destination_register_bitmask = BITMASK_7 | BITMASK_6 | BITMASK_5;
    uint32_t value_bitmask = BITMASK_14 | BITMASK_13 | BITMASK_12 | BITMASK_11 | BITMASK_10 | BITMASK_9 | BITMASK_8;
    uint32_t clear_upper_bits_bitmask = BITMASK_32 | BITMASK_24_TO_1;

    uint32_t destination_register = (word & destination_register_bitmask) >> 4;
    uint32_t value = (word & value_bitmask) << 17;

    cpu_ptr->registers[destination_register] &= clear_upper_bits_bitmask;
    cpu_ptr->registers[destination_register] |= value;
}

void execute_and_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t destination_register_bitmask = BITMASK_7 | BITMASK_6 | BITMASK_5;
    uint32_t first_source_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    uint32_t second_source_register_bitmask = BITMASK_13 | BITMASK_12 | BITMASK_11;
    uint32_t control_bitmask = BITMASK_14;
    uint32_t value_bitmask = BITMASK_32_TO_17 | BITMASK_16 | BITMASK_15;

    uint32_t destination_register = (word & destination_register_bitmask) >> 4;
    uint32_t first_source_register = (word & first_source_register_bitmask) >> 7;
    uint32_t second_source_register = (word & second_source_register_bitmask) >> 10;
    bool use_value_in_place_of_second_register = (word & control_bitmask) >> 13;
    uint32_t value = (word & value_bitmask) >> 14;

    uint32_t return_value;
    if (use_value_in_place_of_second_register) {
        return_value = cpu_ptr->registers[first_source_register] & value;
    } else {
        return_value = cpu_ptr->registers[first_source_register] & cpu_ptr->registers[second_source_register];
    }

    cpu_ptr->registers[destination_register] = return_value;
}

void execute_bsr_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t destination_register_bitmask = BITMASK_7 | BITMASK_6 | BITMASK_5;
    uint32_t source_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    uint32_t bits_to_shift_bitmask = BITMASK_15 | BITMASK_14 | BITMASK_13 | BITMASK_12 | BITMASK_11;
    uint32_t control_bitmask = BITMASK_16;

    uint32_t destination_register = (word & destination_register_bitmask) >> 4;
    uint32_t source_register = (word & source_register_bitmask) >> 7;
    uint32_t bits_to_shift = (word & bits_to_shift_bitmask) >> 10;
    bool rotate_bits = (word & control_bitmask) >> 15;

    uint32_t value;
    if (rotate_bits) {
        value = cpu_ptr->registers[source_register] >> bits_to_shift | cpu_ptr->registers[source_register] << (32 - bits_to_shift);
    } else {
        value = cpu_ptr->registers[source_register] >> bits_to_shift;
    }

    cpu_ptr->registers[destination_register] = value;
}

void execute_bsl_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t destination_register_bitmask = BITMASK_7 | BITMASK_6 | BITMASK_5;
    uint32_t source_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    uint32_t bits_to_shift_bitmask = BITMASK_15 | BITMASK_14 | BITMASK_13 | BITMASK_12 | BITMASK_11;
    uint32_t control_bitmask = BITMASK_16;

    uint32_t destination_register = (word & destination_register_bitmask) >> 4;
    uint32_t source_register = (word & source_register_bitmask) >> 7;
    uint32_t bits_to_shift = (word & bits_to_shift_bitmask) >> 10;
    bool rotate_bits = (word & control_bitmask) >> 15;

    uint32_t value;
    if (rotate_bits) {
        value = cpu_ptr->registers[source_register] << bits_to_shift | cpu_ptr->registers[source_register] >> (32 - bits_to_shift);
    } else {
        value = cpu_ptr->registers[source_register] << bits_to_shift;
    }

    cpu_ptr->registers[destination_register] = value;
}

void execute_instruction(uint32_t word, CPU *cpu_ptr, Memory *memory_ptr) {
    uint32_t op_code = get_op_code(word);
    switch (op_code) {
        case 0:
            execute_jmp_instruction(word, cpu_ptr);
            break;
        case 1:
            execute_ld_instruction(word, cpu_ptr, memory_ptr);
            break;
        case 2:
            execute_put_instruction(word, cpu_ptr, memory_ptr);
            break;
        case 3:
            execute_set_instruction(word, cpu_ptr, memory_ptr);
            break;
        case 4:
            execute_setu_instruction(word, cpu_ptr, memory_ptr);
            break;
        case 10:
            execute_and_instruction(word, cpu_ptr, memory_ptr);
            break;
        case 13:
            execute_bsr_instruction(word, cpu_ptr, memory_ptr);
            break;
        case 14:
            execute_bsl_instruction(word, cpu_ptr, memory_ptr);
            break;
    }
}
