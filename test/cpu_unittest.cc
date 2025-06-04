extern "C" {
#include "../src/bit_utils.h"
#include "../src/cpu.h"
#include "../src/memory.h"
}
#include <gtest/gtest.h>
#include <stdint.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> utils >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static Cpu init_cpu_with_state(uint32_t registers[8]) {
    Cpu cpu = init_cpu();
    cpu.program_counter = 0;
    for (int i = 0; i < 8; i++) {
        cpu.registers[i] = registers[i];
    }
    return cpu;
}

static Memory init_memory_with_state(uint8_t *data, int size) {
    Memory memory = init_memory();
    for (int i = 0; i < size; i++) {
        memory.data[i] = data[i];
    }
    return memory;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> JMP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* When we execute the jump instruction with offset 1 then the program counter should be set to 1 */
TEST(Cpu, test_jmp_intruction_with_offset_value) {
    Cpu cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t jmp_instruction = BITMASK_12 | BITMASK_5 | JMP_BITMASK;
    execute_instruction(jmp_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.program_counter, 1);
}

/*
 * When we execute a jump instruction with an offset of 1 but the "maybe skip instruction" flag is enabled and the
 * "maybe skip instruction" register is true then the program counter should not be set to 1
 */
TEST(Cpu, test_jmp_instruction_skips) {
    uint32_t registers[8] = {0, 1, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t jmp_instruction = BITMASK_12 | BITMASK_6 | BITMASK_5 | BITMASK_4 | JMP_BITMASK;
    execute_instruction(jmp_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.program_counter, 0);
}

/*
 * Attempts to set the program counter to 1 and succeeds because while "maybe skip instruction" flag is enabled our
 * "maybe skip instruction" register is false
 */
TEST(Cpu, test_jmp_instruction_does_not_skip) {
    Cpu cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t jmp_instruction = BITMASK_12 | BITMASK_5 | BITMASK_4 | JMP_BITMASK;
    execute_instruction(jmp_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.program_counter, 1);
}

/* Sets program counter to the value of register 0 plus an offset of 1 */
TEST(Cpu, test_jmp_instruction_works_with_base_register) {
    uint32_t registers[8] = {2, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t jmp_instruction = BITMASK_12 | BITMASK_5 | JMP_BITMASK;
    execute_instruction(jmp_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.program_counter, 3);
}

/* Sets program counter to the value of 1 using a 0 base register plus a 1 offset register */
TEST(Cpu, test_jmp_instruction_works_with_offset_register) {
    uint32_t registers[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t jmp_instruction = BITMASK_12 | JMP_BITMASK;
    execute_instruction(jmp_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.program_counter, 1);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(Cpu, test_st_byte_instruction) {
    uint32_t registers[8] = {0, 1, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    /* Puts the lower order 8 bits of register 1 into memory address 0 */
    uint32_t store_instruction = BITMASK_8 | STB_BITMASK;
    execute_instruction(store_instruction, &cpu, &memory);

    EXPECT_EQ((int) memory.data[0], 1);
}

TEST(Cpu, test_st_4_byte_instruction) {
    uint32_t registers[8] = {0, 0x87654321, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    /* Puts register 1 into memory address 0 to 3 */
    uint32_t store_instruction = BITMASK_15 | BITMASK_14 | BITMASK_8 | BITMASK_7 | STW_BITMASK;
    execute_instruction(store_instruction, &cpu, &memory);

    EXPECT_EQ((int) memory.data[3], 0x21);
    EXPECT_EQ((int) memory.data[2], 0x43);
    EXPECT_EQ((int) memory.data[1], 0x65);
    EXPECT_EQ((int) memory.data[0], 0x87);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(Cpu, test_ld_byte_instruction) {
    Cpu cpu = init_cpu();
    uint8_t data[4] = {0, 0, 0, 1};
    Memory memory = init_memory_with_state(data, 4);

    uint32_t instruction = BITMASK_15 | BITMASK_14 | BITMASK_8 | BITMASK_7 | LDB_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[1], 1);
}

TEST(Cpu, test_ld_4_byte_instruction) {
    Cpu cpu = init_cpu();
    uint8_t data[4] = {0x87, 0x65, 0x43, 0x21};
    Memory memory = init_memory_with_state(data, 4);

    uint32_t instruction = BITMASK_15 | BITMASK_14 | BITMASK_8 | BITMASK_7 | LDW_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[1], 0x87654321);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SET >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(Cpu, test_set_instruction) {
    Cpu cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_8 | SET_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(Cpu, test_set_instruction_with_negative_value) {
    Cpu cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_8 | SETN_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], UINT32_C(4261412865));
}

TEST(Cpu, test_set_instruction_with_different_register) {
    Cpu cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_8 | BITMASK_5 | SET_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[1], 1);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SETU >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(Cpu, test_setu_instruction) {
    Cpu cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_7 | SETU_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 33554432);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ADD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(Cpu, test_add_instruction) {
    uint32_t registers[8] = {1, 2, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_11 | ADD_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/* Adds 2 to register 0 and stores the result in register 0 */
TEST(Cpu, test_add_instruction_with_control_bit) {
    uint32_t registers[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_15 | BITMASK_7 | ADD_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SUB >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Subtracts register 1 from register 0 and stores the result in register 0 */
TEST(Cpu, test_sub_instruction) {
    uint32_t registers[8] = {4, 1, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_14 | SUB_BITMASK;

    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/* Subtracts 1 from register 0 and stores the result in register 0 */
TEST(Cpu, test_sub_instruction_with_control_bit) {
    uint32_t registers[8] = {4, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t instruction = BITMASK_14 | SUBI_BITMASK;
    execute_instruction(instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MUL >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Multiplies register 0 and register 1 and stores the result in register 0 */
TEST(Cpu, test_mul_instruction) {
    uint32_t registers[8] = {2, 3, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t mul_instruction = BITMASK_14 | MUL_BITMASK;
    execute_instruction(mul_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 6);
}

/* Multiplies register 0 by 5 and stores the result in register 0 */
TEST(Cpu, test_mul_instruction_with_control_bit) {
    uint32_t registers[8] = {2, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t mul_instruction = BITMASK_16 | BITMASK_14 | MULI_BITMASK;
    execute_instruction(mul_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 10);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DIV >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Divides register 0 by register 1 and stores the result in register 0 */
TEST(Cpu, test_div_instruction) {
    uint32_t registers[8] = {6, 2, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t div_instruction = BITMASK_14 | DIV_BITMASK;
    execute_instruction(div_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/* Divides register 0 by 2 and stores the result in register 0 */
TEST(Cpu, test_div_instruction_with_control_bit) {
    uint32_t registers[8] = {6, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t div_instruction = BITMASK_15 | DIVI_BITMASK;
    execute_instruction(div_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MOD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Performs the modulo operation on register 0 using register 1 and stores the remainder in register 0 */
TEST(Cpu, test_mod_instruction) {
    uint32_t registers[8] = {6, 5, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t mod_instruction = BITMASK_14 | MOD_BITMASK;
    execute_instruction(mod_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}

/* Performs the modulo operation on register 0 using a value of 5 and stores the remainder in register 0 */
TEST(Cpu, test_mod_instruction_with_control_bit) {
    uint32_t registers[8] = {6, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t mod_instruction = BITMASK_16 | BITMASK_14 | MODI_BITMASK;
    execute_instruction(mod_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> AND >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Performs an AND operation on register 0 and register 1 */
TEST(Cpu, test_and_instruction) {
    uint32_t registers[8] = {0b0011, 0b0101, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t and_instruction = BITMASK_11 | AND_BITMASK;
    execute_instruction(and_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0001);
}

/* Performs an AND operation on register 0 and value 101 */
TEST(Cpu, test_and_instruction_with_control_bit) {
    uint32_t registers[8] = {0b0110, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t and_instruction = BITMASK_16 | BITMASK_14 | ANDI_BITMASK;
    execute_instruction(and_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0100);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> OR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Performs an OR operation on register 0 and register 1 */
TEST(Cpu, test_or_instruction) {
    uint32_t registers[8] = {0b0011, 0b0101, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t or_instruction = BITMASK_11 | OR_BITMASK;
    execute_instruction(or_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0111);
}

/* Performs an OR operation on register 0 and value 0b100 */
TEST(Cpu, test_or_instruction_with_control_bit) {
    uint32_t registers[8] = {0b1010, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t or_instruction = BITMASK_16 | ORI_BITMASK;
    execute_instruction(or_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b1110);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> XOR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Performs an XOR operation on register 0 and register 1 */
TEST(Cpu, test_xor_instruction) {
    uint32_t registers[8] = {0b011, 0b0101, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t xor_instruction = BITMASK_11 | XOR_BITMASK;
    execute_instruction(xor_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0110);
}

/* Performs an XOR operation on register 0 and value 0b0010 */
TEST(Cpu, test_xor_instruction_with_control_bit) {
    uint32_t registers[8] = {0b1010, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t xor_instruction = BITMASK_15 | XORI_BITMASK;
    execute_instruction(xor_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b1000);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BSR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Bit shifts register 0 by register 1 and stores the value in register 0 */
TEST(Cpu, test_bsr_instruction) {
    uint32_t registers[8] = {0b0100, 2, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t bsr_instruction = BITMASK_13 | BSR_BITMASK;
    execute_instruction(bsr_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0001);
}

/* Bit shifts register 0 twice resulting in a value of 1 */
TEST(Cpu, test_bsr_instruction_with_control_bit) {
    uint32_t registers[8] = {0b0100, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t bsr_instruction = BITMASK_14 | BSRI_BITMASK;
    execute_instruction(bsr_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0001);
}

/* 
 * Bit shifts register 0 twice while letting it overflow back onto the higher order bits resulting in a value of
 * -1073741823 
 */
TEST(Cpu, test_bsr_instruction_with_control_bit_and_overflow) {
    uint32_t registers[8] = {0b0101, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t bsr_instruction = BITMASK_14 | BSRRI_BITMASK;
    execute_instruction(bsr_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], BITMASK_31 | BITMASK_1);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BSL >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* Bit shifts register 0 by register 1 and stores the value in register 0 */
TEST(Cpu, test_bsl_instruction) {
    uint32_t registers[8] = {0b0010, 2, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t bsl_instruction = BITMASK_13 | BSL_BITMASK;
    execute_instruction(bsl_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b1000);
}

/* Bit shifts register 0 left twice resulting in a value of 0b1000 */
TEST(Cpu, test_bsl_instruction_with_control_bit) {
    uint32_t registers[8] = {0b0010, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t bsl_instruction = BITMASK_14 | BSLI_BITMASK;
    execute_instruction(bsl_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b1000);
}

/* Bit shifts register 0 once while letting it overflow back onto the lower order bits resulting in a value of 3 */
TEST(Cpu, test_bsl_instruction_with_control_bit_and_overflow) {
    uint32_t registers[8] = {BITMASK_32 | BITMASK_1, 0, 0, 0, 0, 0, 0, 0};
    Cpu cpu = init_cpu_with_state(registers);
    Memory memory = init_memory();

    uint32_t bsl_instruction = BITMASK_13 | BSLRI_BITMASK;
    execute_instruction(bsl_instruction, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 0b0011);
}
