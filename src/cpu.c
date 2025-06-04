/*********************************************************************************************************************
 * Simulates a CPU using a 32 bit word size                                                                          *
 *                                                                                                                   *
 * Instruction set and list of registers are outlined in the ARCHITECTURE_SPECIFICATIONS.txt file                    *
 *********************************************************************************************************************/

#include "cpu.h"
#include "bit_utils.h"
#include "memory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_REGISTERS 8

Cpu init_cpu() {
    Cpu cpu = {
        0,                       // Program counter
        {0, 0, 0, 0, 0, 0, 0, 0} // Registers
    };
    return cpu;
}

char get_op_code(uint32_t word) {
    return word & OP_CODE_BITMASK;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Utils >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void fail_if_invalid_register(uint32_t register_number) {
    if (register_number >= NUM_REGISTERS) {
        fprintf(stderr, "Invalid register access, register %d does not exist.", register_number);
        exit(EXIT_FAILURE);
    }
}

void fail_if_invalid_memory_location(uint32_t location) {
    if (location >= MEMORY_SIZE_BYTES) {
        fprintf(stderr, "Invalid memory access, memory locatiom %d does not exist.\n", location);
        exit(EXIT_FAILURE);
    }
}

void fail_if_invalid_byte_mode(uint32_t byte_mode) {
    if (byte_mode > 2) {
        fprintf(stderr, "Invalid byte mode %d\n", byte_mode);
        exit(EXIT_FAILURE);
    }
}

/* This looks a bit awkward but this is because our address space starts at 0 */
void fail_if_invalid_memory_alignment(uint32_t location, uint32_t byte_mode) {
    if ((byte_mode == 1 && location % 2 != 1) || (byte_mode == 2 && location % 4 != 3)) {
        fprintf(stderr, "Invalid memory alignment, byte mode %d and location %d\n", byte_mode, location);
        exit(EXIT_FAILURE);
    }
}

void fail_if_memory_underflow(uint32_t location, uint32_t byte_mode) {
    if ((location < 3 && byte_mode == 2) || (location < 1 && byte_mode == 1)) {
        fprintf(stderr, "Memory underflow, location %d and byte mode %d\n", location, byte_mode);
        exit(EXIT_FAILURE);
    }
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> JMP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void execute_jmp_instruction(uint32_t word, Cpu *cpu) {
    /* Bitmasks for the instruction */
    const uint32_t maybe_skip_instruction_bitmask = BITMASK_4;
    const uint32_t use_higher_order_bits_as_offset_value_bitmask = BITMASK_5;
    const uint32_t maybe_skip_instruction_register_bitmask = BITMASK_8 | BITMASK_7 | BITMASK_6;
    const uint32_t base_register_bitmask = BITMASK_11 | BITMASK_10 | BITMASK_9;
    const uint32_t offset_register_or_value_bitmask = BITMASK_32_TO_13 | BITMASK_12;

    /* Extracted values for the instruction */
    bool maybe_skip_instruction = (word & maybe_skip_instruction_bitmask) >> 3;
    bool use_higher_order_bits_as_offset_value = (word & use_higher_order_bits_as_offset_value_bitmask) >> 4;
    uint32_t maybe_skip_instruction_register = (word & maybe_skip_instruction_register_bitmask) >> 5;
    uint32_t base_register = (word & base_register_bitmask) >> 8;
    uint32_t offset_register_or_value = (word & offset_register_or_value_bitmask) >> 11;

    /* Determine if the instruction should be skipped */
    if (maybe_skip_instruction && cpu->registers[maybe_skip_instruction_register]) {
        return;
    }

    /* Determine the new program counter value */
    if (use_higher_order_bits_as_offset_value) {
        cpu->program_counter = cpu->registers[base_register] + offset_register_or_value;
    } else {
        cpu->program_counter = cpu->registers[base_register] + cpu->registers[offset_register_or_value];
    }
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ST / LD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Forward declare functions so they are logically ordered and easier to read */
void store_value_in_memory(uint32_t, uint32_t, uint32_t, Memory *);
uint32_t load_value_from_memory(uint32_t, uint32_t, Cpu *, Memory *);
uint32_t get_offset(uint32_t, uint32_t, bool, Cpu *);

void execute_memory_management_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    const uint32_t operation_bitmask = BITMASK_4;
    const uint32_t byte_mode_bitmask = BITMASK_6 | BITMASK_5;
    const uint32_t use_upper_bits_as_offset_bitmask = BITMASK_7;
    const uint32_t destination_or_source_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    const uint32_t base_register_bitmask = BITMASK_13 | BITMASK_12 | BITMASK_11;
    const uint32_t offset_register_or_value_bitmask = BITMASK_32_TO_17 | BITMASK_16 | BITMASK_15 | BITMASK_14;

    /* 0 = store, 1 = load */
    uint32_t operation = (word & operation_bitmask) >> 3;

    uint32_t byte_mode = (word & byte_mode_bitmask) >> 4;
    fail_if_invalid_byte_mode(byte_mode);
    bool use_upper_bits_as_offset = (word & use_upper_bits_as_offset_bitmask) >> 6;
    uint32_t destination_or_source_register = (word & destination_or_source_register_bitmask) >> 7;
    uint32_t base_register = (word & base_register_bitmask) >> 10;
    uint32_t offset = get_offset(word, offset_register_or_value_bitmask, use_upper_bits_as_offset, cpu);
    uint32_t location = cpu->registers[base_register] + offset;
    fail_if_invalid_memory_location(location);

    if (operation == 0) {
        store_value_in_memory(cpu->registers[destination_or_source_register], location, byte_mode, memory);
    } else {
        cpu->registers[destination_or_source_register] = load_value_from_memory(location, byte_mode, cpu, memory);
    }
}

uint32_t get_offset(uint32_t word, uint32_t offset_register_or_value_bitmask, bool use_upper_bits_as_offset, Cpu *cpu) {
    uint32_t offset_register_or_value = (word & offset_register_or_value_bitmask) >> 13;
    uint32_t offset;
    if (use_upper_bits_as_offset) {
        offset = offset_register_or_value;
    } else {
        fail_if_invalid_register(offset_register_or_value);
        offset = cpu->registers[offset_register_or_value];
    }
    return offset;
}

/* Uses deliberate fallthrough */
void store_value_in_memory(uint32_t value, uint32_t location, uint32_t byte_mode, Memory *memory) {
    switch (byte_mode) {
    case 2:
        memory->data[location - 3] = (value & 0xFF000000) >> 24;
        memory->data[location - 2] = (value & 0x00FF0000) >> 16;
    case 1:
        memory->data[location - 1] = (value & 0x0000FF00) >> 8;
    case 0:
        memory->data[location] = value & 0x000000FF;
    }
}

/* Loads a value from memory, given the location and byte mode */
uint32_t load_value_from_memory(uint32_t location, uint32_t byte_mode, Cpu *cpu, Memory *memory) {
    fail_if_invalid_memory_location(location);
    fail_if_memory_underflow(location, byte_mode);
    fail_if_invalid_memory_alignment(location, byte_mode);

    uint32_t value = 0;
    switch (byte_mode) {
    case 2:
        value |= memory->data[location - 3] << 24;
        value |= memory->data[location - 2] << 16;
    case 1:
        value |= memory->data[location - 1] << 8;
    case 0:
        value |= memory->data[location];
    }
    return value;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SET / SETU >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void execute_set_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    /* Bitmasks for the instruction */
    const uint32_t set_negative_value_bitmask = BITMASK_4;
    const uint32_t destination_register_bitmask = BITMASK_7 | BITMASK_6 | BITMASK_5;
    const uint32_t value_bitmask = BITMASK_32_TO_9 | BITMASK_8;

    /* Extracted values for the instruction */
    bool set_negative_value = (word & set_negative_value_bitmask) >> 3;
    uint32_t destination_register = (word & destination_register_bitmask) >> 4;
    uint32_t value = (word & value_bitmask) >> 7;

    if (set_negative_value) {
        value |= BITMASK_32_TO_29 | BITMASK_28 | BITMASK_27 | BITMASK_26;
    }
    cpu->registers[destination_register] = value;
}

void execute_setu_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    const uint32_t destination_register_bitmask = BITMASK_6 | BITMASK_5 | BITMASK_4;
    const uint32_t value_bitmask = BITMASK_12 | BITMASK_11 | BITMASK_10 | BITMASK_9 | BITMASK_8 | BITMASK_7;
    const uint32_t clear_upper_bits_bitmask = BITMASK_32 | BITMASK_25 | BITMASK_24_TO_1;

    uint32_t destination_register = (word & destination_register_bitmask) >> 3;
    uint32_t value = (word & value_bitmask) << 19;

    cpu->registers[destination_register] &= clear_upper_bits_bitmask;
    cpu->registers[destination_register] |= value;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ADD / SUB / MUL / DIV / MOD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void execute_arithmetic_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    const uint32_t arithmetic_op_code_bitmask = BITMASK_6 | BITMASK_5 | BITMASK_4;
    const uint32_t use_upper_bits_as_value_bitmask = BITMASK_7;
    const uint32_t destination_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    const uint32_t first_source_register_bitmask = BITMASK_13 | BITMASK_12 | BITMASK_11;
    const uint32_t value_or_second_source_register_bitmask = BITMASK_32_TO_17 | BITMASK_16 | BITMASK_15 | BITMASK_14;

    uint32_t arithmetic_op_code = (word & arithmetic_op_code_bitmask) >> 3;
    bool use_upper_bits_as_value = (word & use_upper_bits_as_value_bitmask) >> 6;
    uint32_t destination_register = (word & destination_register_bitmask) >> 7;
    uint32_t first_source_register = (word & first_source_register_bitmask) >> 10;
    uint32_t value_or_second_source_register = (word & value_or_second_source_register_bitmask) >> 13;

    uint32_t value =
        use_upper_bits_as_value ? value_or_second_source_register : cpu->registers[value_or_second_source_register];
    uint32_t return_value;
    switch (arithmetic_op_code) {
    case 0:
        return_value = cpu->registers[first_source_register] + value;
        break;
    case 1:
        return_value = cpu->registers[first_source_register] - value;
        break;
    case 2:
        return_value = cpu->registers[first_source_register] * value;
        break;
    case 3:
        return_value = cpu->registers[first_source_register] / value;
        break;
    case 4:
        return_value = cpu->registers[first_source_register] % value;
        break;
    default:
        fprintf(stderr, "Invalid arithmetic operation %d\n", arithmetic_op_code);
        exit(EXIT_FAILURE);
    }
    cpu->registers[destination_register] = return_value;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> AND / OR / XOR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void execute_bitwise_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    const uint32_t bitwise_op_code_bitmask = BITMASK_5 | BITMASK_4;
    const uint32_t use_upper_bits_as_value_bitmask = BITMASK_6;
    const uint32_t fill_unset_bits_bitmask = BITMASK_7;
    const uint32_t destination_register_bitmask = BITMASK_10 | BITMASK_9 | BITMASK_8;
    const uint32_t first_source_register_bitmask = BITMASK_13 | BITMASK_12 | BITMASK_11;
    const uint32_t value_or_second_source_register_bitmask = BITMASK_32_TO_17 | BITMASK_16 | BITMASK_15 | BITMASK_14;

    uint32_t bitwise_op_code = (word & bitwise_op_code_bitmask) >> 3;
    bool use_upper_bits_as_value = (word & use_upper_bits_as_value_bitmask) >> 5;
    bool fill_upper_bits = (word & fill_unset_bits_bitmask) >> 6;
    uint32_t destination_register = (word & destination_register_bitmask) >> 7;
    uint32_t first_source_register = (word & first_source_register_bitmask) >> 10;
    uint32_t value_or_second_source_register = (word & value_or_second_source_register_bitmask) >> 13;

    uint32_t value =
        use_upper_bits_as_value ? value_or_second_source_register : cpu->registers[value_or_second_source_register];
    uint32_t return_value;
    switch (bitwise_op_code) {
    case 0:
        return_value = cpu->registers[first_source_register] & value;
        break;
    case 1:
        return_value = cpu->registers[first_source_register] | value;
        break;
    case 2:
        return_value = cpu->registers[first_source_register] ^ value;
        break;
    default:
        fprintf(stderr, "Invalid bitwise operation %d\n", bitwise_op_code);
        exit(EXIT_FAILURE);
    }

    cpu->registers[destination_register] = return_value;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BSR / BSL >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void execute_bitshift_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    const uint32_t bitshift_op_code_bitmask = BITMASK_4;
    const uint32_t use_upper_bits_as_value_bitmask = BITMASK_5;
    const uint32_t rotate_bits_bitmask = BITMASK_6;
    uint32_t destination_register_bitmask = BITMASK_9 | BITMASK_8 | BITMASK_7;
    uint32_t first_source_register_bitmask = BITMASK_12 | BITMASK_11 | BITMASK_10;
    uint32_t value_or_second_register_bitmask = BITMASK_17 | BITMASK_16 | BITMASK_15 | BITMASK_14 | BITMASK_13;

    uint32_t bitshift_op_code = (word & bitshift_op_code_bitmask) >> 3;
    bool use_upper_bits_as_value = (word & use_upper_bits_as_value_bitmask) >> 4;
    bool rotate_bits = (word & rotate_bits_bitmask) >> 5;
    uint32_t destination_register = (word & destination_register_bitmask) >> 6;
    uint32_t first_source_register = (word & first_source_register_bitmask) >> 9;
    uint32_t value_or_second_register = (word & value_or_second_register_bitmask) >> 12;

    uint32_t value = use_upper_bits_as_value ? value_or_second_register : cpu->registers[value_or_second_register];
    if (bitshift_op_code) {
        /* BSL */
        if (rotate_bits) {
            value =
                cpu->registers[first_source_register] << value | cpu->registers[first_source_register] >> (32 - value);
        } else {
            value = cpu->registers[first_source_register] << value;
        }
    } else {
        if (rotate_bits) {
            value = cpu->registers[first_source_register] >> value | 
                cpu->registers[first_source_register] << (32 - value);
        } else {
            value = cpu->registers[first_source_register] >> value;
        }
    }
    cpu->registers[destination_register] = value;
}

void execute_instruction(uint32_t word, Cpu *cpu, Memory *memory) {
    uint32_t op_code = get_op_code(word);
    switch (op_code) {
    case 0:
        execute_jmp_instruction(word, cpu);
        break;
    case 1:
        execute_memory_management_instruction(word, cpu, memory);
        break;
    case 2:
        execute_set_instruction(word, cpu, memory);
        break;
    case 3:
        execute_setu_instruction(word, cpu, memory);
        break;
    case 4:
        execute_arithmetic_instruction(word, cpu, memory);
        break;
    case 5:
        execute_bitwise_instruction(word, cpu, memory);
        break;
    case 6:
        execute_bitshift_instruction(word, cpu, memory);
        break;
    }
}
