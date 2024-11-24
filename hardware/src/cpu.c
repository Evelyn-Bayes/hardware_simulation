/*
 * Simulates a CPU using a 32 bit word size
 * Instruction set and list of registers are outlined in the SPECIFICATIONS.txt doc.
*/

#include <stdio.h>
#include <stdlib.h>

const short CYCLES_PER_SECOND = 10000;

// L1 Cache
const char L1_CACHE_CYCLE_COST = 10;
const int L1_CACHE_BYTE_SIZE = 65536;

// L2 Cache
const char L2_CACHE_CYCLE_COST = 100;
const int L2_CACHE_BYTE_SIZE = 262144;

// Main Memory
const short MAIN_MEMORY_CYCLE_COST = 1000;
const int MAIN_MEMORY_BYTE_SIZE = 1048576;

// Cache / Memory
char l1_cache[L1_CACHE_BYTE_SIZE];
char l2_cache[L2_CACHE_BYTE_SIZE];
char main_memory[MAIN_MEMORY_BYTE_SIZE];

// Program Counter
int PC;

// Registers
int registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};

char get_op_code(int word) {
    int op_code_bit_filter = 0b11110000000000000000000000000000;
    return (word & op_code_bit_filter) >> 28;
}


void execute_set_instruction(int word, int registers[]) {
    int register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int value = word & 0b00000001111111111111111111111111;
    registers[register_index] = value;
}

void execute_setu_instruction(int word, int registers[]) {
    int register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int value = (word & 0b00000001111111100000000000000000) << 7;
    registers[register_index] |= value;
}

void execute_add_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] + value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] + registers[source_register_two_index];
    }
}

void execute_sub_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] - value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] - registers[source_register_two_index];
    }
}

void execute_mul_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] * value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] * registers[source_register_two_index];
    }
}

void execute_div_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] / value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] / registers[source_register_two_index];
    }
}

void execute_mod_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] % value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] % registers[source_register_two_index];
    }
}

void execute_and_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] & value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] & registers[source_register_two_index];
    }
}

void execute_or_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] | value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] | registers[source_register_two_index];
    }
}

void execute_xor_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_one_index = (word & 0b00000001110000000000000000000000) >> 22;
    int source_register_two_index = (word & 0b00000000001110000000000000000000) >> 19;

    int use_source_register_one_as_destination = (word & 0b00000000000000100000000000000000) >> 17;
    destination_register_index = use_source_register_one_as_destination ? source_register_two_index : destination_register_index; 

    int use_lower_bits_instead_of_register_two = (word & 0b00000000000001000000000000000000) >> 18;
    if (use_lower_bits_instead_of_register_two) {
        int value = (word & 0b00000000000000011111111111111111);
        registers[destination_register_index] = registers[source_register_one_index] ^ value;
    } else {
        registers[destination_register_index] = registers[source_register_one_index] ^ registers[source_register_two_index];
    }
}

void execute_bsr_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_index = (word & 0b00000001110000000000000000000000) >> 22;
    int bits_to_shift = (word & 0b00000000001111000000000000000000) >> 18;

    int use_source_register_as_destination = (word & 0b00000000000000001000000000000000) >> 15;
    destination_register_index = use_source_register_as_destination ? source_register_index : destination_register_index;

    int set_lower_bits_to_zero = (word & 0b00000000000000010000000000000000) >> 16;
    if (set_lower_bits_to_zero) {
        registers[destination_register_index] = registers[source_register_index] << bits_to_shift;
    } else {
        registers[destination_register_index] = registers[source_register_index] << bits_to_shift | registers[source_register_index] >> (32 - bits_to_shift);
    }
}

void execute_bsl_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int source_register_index = (word & 0b00000001110000000000000000000000) >> 22;
    int bits_to_shift = (word & 0b00000000001111000000000000000000) >> 18;

    int use_source_register_as_destination = (word & 0b00000000000000001000000000000000) >> 15;
    destination_register_index = use_source_register_as_destination ? source_register_index : destination_register_index;

    int set_upper_bits_to_zero = (word & 0b00000000000000010000000000000000) >> 16;
    if (set_upper_bits_to_zero) {
        registers[destination_register_index] = registers[source_register_index] >> bits_to_shift;
    } else {
        registers[destination_register_index] = registers[source_register_index] >> bits_to_shift | registers[source_register_index] << (32 - bits_to_shift);
    }
}

void execute_ld_instruction(int word, int registers[]) {
    int destination_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int address_register_index = (word & 0b00000001110000000000000000000000) >> 22;
    int memory_address = word & 0b00000000000111111111111111111111;

    int use_lower_bits_as_address = (word &0b00000000001000000000000000000000) >> 21;
    if (!use_lower_bits_as_address) {
        int memory_address = registers[address_register_index];
    }
    registers[destination_register_index] = main_memory[memory_address] << 24 | 
                                                            main_memory[memory_address + 1] << 16 | 
                                                            main_memory[memory_address + 2] << 8 | 
                                                            main_memory[memory_address + 3];
}

void execute_put_instruction(int word, int registers[]) {
    int value_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int address_register_index = (word & 0b00000001110000000000000000000000) >> 22;
    int memory_address = word & 0b00000000000001111111111111111111;

    int use_lower_bits_as_address = (word &0b00000000001000000000000000000000) >> 21;
    if (!use_lower_bits_as_address) {
        int memory_address = registers[address_register_index];
    }
    main_memory[memory_address] = registers[value_register_index] >> 24;
    main_memory[memory_address + 1] = registers[value_register_index] >> 16;
    main_memory[memory_address + 2] = registers[value_register_index] >> 8;
    main_memory[memory_address + 3] = registers[value_register_index];
}

void execute_jmp_instruction(int word, int registers[]) {
    int intruction_register_index = (word & 0b00001110000000000000000000000000) >> 25;
    int use_lower_bits_as_value = (word &0b00000001000000000000000000000000) >> 24;
    int value = word & 0b00000000111111111111111111111111;
    if (use_lower_bits_as_value) {
        PC = value;
    } else {
        PC = registers[intruction_register_index];
    }
}

void execute_instruction(int word) {
    int op_code = get_op_code(word);
    switch (op_code) {
        case 0:
            execute_set_instruction(word, registers);
            break;
        case 1:
            execute_setu_instruction(word, registers);
            break;
        case 2:
            execute_add_instruction(word, registers);
            break;
        case 3:
            execute_sub_instruction(word, registers);
            break;
        case 4:
            execute_mul_instruction(word, registers);
            break;
        case 5:
            execute_div_instruction(word, registers);
            break;
        case 6:
            execute_mod_instruction(word, registers);
            break;
        case 7:
            execute_and_instruction(word, registers);
            break;
        case 8:
            execute_or_instruction(word, registers);
            break;
        case 9:
            execute_xor_instruction(word, registers);
            break;
        case 10:
            execute_bsr_instruction(word, registers);
            break;
        case 11:
            execute_bsl_instruction(word, registers);
            break;
        case 12:
            execute_ld_instruction(word, registers);
            break;
        case 13:
            execute_put_instruction(word, registers);
            break;
        case 14:
            execute_jmp_instruction(word, registers);
            break;
        default:
            fprintf(stderr, "Invalid op code: %d\n", op_code);
            exit(1);
    }
}
