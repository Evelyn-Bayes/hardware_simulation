extern "C" {
    #include "../src/cpu.h"
    #include "../src/memory.h"
    #include "../src/bit_utils.h"
}
#include <stdint.h>
#include <gtest/gtest.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> JMP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_jmp_intruction_with_fixed_value_offset) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t word = BITMASK_13 | BITMASK_5 | JMP_BITMASK;
    // Set the program counter to 1
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_jmp_instruction_skips_using_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t word = BITMASK_13 | BITMASK_9 | BITMASK_5 | JMP_BITMASK;

    /* Attempts to set the program counter to 1 but skips because control bit is set and control register is 0 */
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 0);
}

TEST(CPU, test_jmp_instruction_doesnt_skip_using_control_bit_and_positive_control_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    uint32_t jmp_word = BITMASK_13 | BITMASK_10 | BITMASK_9 | BITMASK_5 | JMP_BITMASK;

    /* Sets register 1 to 1 */
    execute_instruction(set_word, &cpu, &memory);
    /* Attempts to set the program counter to 1 and succeeds because control bit is set and control register is 1 */
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_jmp_instruction_works_with_base_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_10 | SET_BITMASK;
    uint32_t jmp_word = BITMASK_13 | BITMASK_5 | JMP_BITMASK;

    /* Sets register 0 to 2 */
    execute_instruction(set_word, &cpu, &memory);
    /* Sets program counter to the value of register 0 plus an offset of 1 */
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 3);
}

TEST(CPU, test_jmp_instruction_works_with_offset_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    uint32_t jmp_word = BITMASK_13 | JMP_BITMASK;

    /* Sets register 1 to 1 */
    execute_instruction(set_word, &cpu, &memory);
    /* Sets program counter to the value of 1 using a 0 base register plus a 1 offset register */
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PUT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_put_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    uint32_t put_word = BITMASK_8 | PUT_BITMASK;

    /* Sets register 1 to 1 */
    execute_instruction(set_word, &cpu, &memory);
    /* Puts the lower order 8 bits of register 1 into memory address 0 */
    execute_instruction(put_word, &cpu, &memory);
    EXPECT_EQ((int) memory.data[0], 1);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SET >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_set_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t word = BITMASK_9 | SET_BITMASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_set_instruction_with_negative_value) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t word = BITMASK_9 | BITMASK_8 | SET_BITMASK;
    execute_instruction(word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], -16777215);
}

TEST(CPU, test_set_instruction_with_different_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[1], 1);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SETU >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_setu_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t word = BITMASK_8 | SETU_BITMASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 16777216);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ADD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_add_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word_1 = BITMASK_9 | SET_BITMASK;
    uint32_t set_word_2 = BITMASK_10 | BITMASK_5 | SET_BITMASK;
    uint32_t add_word = BITMASK_11 | ADD_BITMASK;

    /* Sets register 0 to 1 */
    execute_instruction(set_word_1, &cpu, &memory);
    /* Sets register 1 to 2 */
    execute_instruction(set_word_2, &cpu, &memory);
    /* Adds register 0 and register 1 and stores the result in register 0 */
    execute_instruction(add_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

TEST(CPU, test_add_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_9 | SET_BITMASK;
    uint32_t add_word = BITMASK_16 | BITMASK_14 | ADD_BITMASK;

    /* Sets register 0 to 1 */
    execute_instruction(set_word, &cpu, &memory);
    /* Adds 2 tp register 0 and stores the result in register 0 */
    execute_instruction(add_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SUB >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_sub_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word_1 = BITMASK_11 | SET_BITMASK;
    uint32_t set_word_2 = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    uint32_t sub_word = BITMASK_11 | SUB_BITMASK;

    /* Sets register 0 to 4 */
    execute_instruction(set_word_1, &cpu, &memory);
    /* Sets register 1 to 1 */
    execute_instruction(set_word_2, &cpu, &memory);
    /* Subtracts register 1 from register 0 and stores the result in register 0 */
    execute_instruction(sub_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

TEST(CPU, test_sub_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_11 | SET_BITMASK;
    uint32_t sub_word = BITMASK_15 | BITMASK_14 | SUB_BITMASK;

    /* Sets register 0 to 4 */
    execute_instruction(set_word, &cpu, &memory);
    /* Subtracts 1 from register 0 and stores the result in register 0 */
    execute_instruction(sub_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MUL >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_mul_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word_1 = BITMASK_10 | SET_BITMASK;
    uint32_t set_word_2 = BITMASK_10 | BITMASK_9 | BITMASK_5 | SET_BITMASK;
    uint32_t mul_word = BITMASK_11 | MUL_BITMASK;

    /* Sets register 0 to 2 */
    execute_instruction(set_word_1, &cpu, &memory);
    /* Sets register 1 to 3 */
    execute_instruction(set_word_2, &cpu, &memory);
    /* Multiplies register 0 and register 1 and stores the result in register 0 */
    execute_instruction(mul_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 6);
}

TEST(CPU, test_mul_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_10 | SET_BITMASK;
    uint32_t mul_word = BITMASK_17 | BITMASK_15 | BITMASK_14 | MUL_BITMASK;

    /* Sets register 0 to 2 */
    execute_instruction(set_word, &cpu, &memory);
    /* Multiplies register 0 by 5 and stores the result in register 0 */
    execute_instruction(mul_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 10);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DIV >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_div_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word_1 = BITMASK_11 | BITMASK_10 | SET_BITMASK;
    uint32_t set_word_2 = BITMASK_10 | BITMASK_5 | SET_BITMASK;
    uint32_t div_word = BITMASK_11 | DIV_BITMASK;

    /* Sets register 0 to 6 */
    execute_instruction(set_word_1, &cpu, &memory);
    /* Sets register 1 to 2 */
    execute_instruction(set_word_2, &cpu, &memory);
    /* Divides register 0 by register 1 and stores the result in register 0 */
    execute_instruction(div_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

TEST(CPU, test_div_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_11 | BITMASK_10 | SET_BITMASK;
    uint32_t div_word = BITMASK_16 | BITMASK_14 | DIV_BITMASK;

    /* Sets register 0 to 6 */
    execute_instruction(set_word, &cpu, &memory);
    /* Divides register 0 by 2 and stores the result in register 0 */
    execute_instruction(div_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 3);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MOD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_mod_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word_1 = BITMASK_11 | BITMASK_10 | SET_BITMASK;
    uint32_t set_word_2 = BITMASK_11 | BITMASK_9 | BITMASK_5 | SET_BITMASK;
    uint32_t mod_word = BITMASK_11 | MOD_BITMASK;

    /* Sets register 0 to 6 */
    execute_instruction(set_word_1, &cpu, &memory);
    /* Sets register 1 to 5 */
    execute_instruction(set_word_2, &cpu, &memory);
    /* Performs the modulo operation on register 0 using register 1 and stores the remainder in register 0 */
    execute_instruction(mod_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_mod_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_11 | BITMASK_10 | SET_BITMASK;
    uint32_t mod_word = BITMASK_17 | BITMASK_15 | BITMASK_14 | MOD_BITMASK;

    /* Sets register 0 to 6 */
    execute_instruction(set_word, &cpu, &memory);
    /* Performs the modulo operation on register 0 using a value of 5 and stores the remainder in register 0 */
    execute_instruction(mod_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> AND >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_and_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 00000011 */
    uint32_t set_word_1 = BITMASK_10 | BITMASK_9 | SET_BITMASK;

    /* Sets the lower byte of register 1 to 00000101 */
    uint32_t set_word_2 = BITMASK_11 | BITMASK_9 | BITMASK_5 | SET_BITMASK;

    /* Performs an AND operation on register 0 and register 1 */
    uint32_t and_word = BITMASK_11 | AND_BITMASK;

    execute_instruction(set_word_1, &cpu, &memory);
    execute_instruction(set_word_2, &cpu, &memory);
    execute_instruction(and_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_and_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 00000110 */
    uint32_t set_word = BITMASK_11 | BITMASK_10 | SET_BITMASK;

    /* Performs an AND operation on register 0 and value 101 */
    uint32_t and_word = BITMASK_17 | BITMASK_15 | BITMASK_14 | AND_BITMASK;

    execute_instruction(set_word, &cpu, &memory);
    execute_instruction(and_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 4);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> OR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_or_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 0b11 */
    uint32_t set_word_1 = BITMASK_10 | BITMASK_9 | SET_BITMASK;

    /* Sets the lower byte of register 1 to 0b101 */
    uint32_t set_word_2 = BITMASK_11 | BITMASK_9 | BITMASK_5 | SET_BITMASK;

    /* Performs an OR operation on register 0 and register 1 */
    uint32_t or_word = BITMASK_11 | OR_BITMASK;

    execute_instruction(set_word_1, &cpu, &memory);
    execute_instruction(set_word_2, &cpu, &memory);
    execute_instruction(or_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 7);
}

TEST(CPU, test_or_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 0b1010 */
    uint32_t set_word = BITMASK_12 | BITMASK_10 | SET_BITMASK;

    /* Performs an OR operation on register 0 and value 0b100 */
    uint32_t or_word = BITMASK_17 | BITMASK_14 | OR_BITMASK;

    execute_instruction(set_word, &cpu, &memory);
    execute_instruction(or_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 14);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> XOR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_xor_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 0b11 */
    uint32_t set_word_1 = BITMASK_10 | BITMASK_9 | SET_BITMASK;

    /* Sets the lower byte of register 1 to 0b101 */
    uint32_t set_word_2 = BITMASK_11 | BITMASK_9 | BITMASK_5 | SET_BITMASK;

    /* Performs an XOR operation on register 0 and register 1 */
    uint32_t xor_word = BITMASK_11 | XOR_BITMASK;

    execute_instruction(set_word_1, &cpu, &memory);
    execute_instruction(set_word_2, &cpu, &memory);
    execute_instruction(xor_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 6);
}

TEST(CPU, test_xor_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 0b1010 */
    uint32_t set_word = BITMASK_12 | BITMASK_10 | SET_BITMASK;

    /* Performs an XOR operation on register 0 and value 0b10 */
    uint32_t xor_word = BITMASK_16 | BITMASK_14 | XOR_BITMASK;

    execute_instruction(set_word, &cpu, &memory);
    execute_instruction(xor_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 8);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BSR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_bsr_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_11 | SET_BITMASK;
    uint32_t bsr_word = BITMASK_12 | BSR_BITMASK;

    /* Sets register 0 to 4 */
    execute_instruction(set_word, &cpu, &memory);

    /* Bit shifts register 0 twice resulting in a value of 1 */
    execute_instruction(bsr_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_bsr_instruction_with_overflow) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_11 | BITMASK_10 | BITMASK_9 | SET_BITMASK;
    uint32_t bsr_word = BITMASK_16 | BITMASK_12 | BSR_BITMASK;

    /* Sets register 0 to 5 */
    execute_instruction(set_word, &cpu, &memory);

    /* Bit shifts register 0 twice while letting it overflow back onto the higher order bits resulting in a value of -1073741823 */
    execute_instruction(bsr_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], -1073741823);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BSL >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_bsl_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_10 | SET_BITMASK;
    uint32_t bsl_word = BITMASK_12 | BSL_BITMASK;

    /* Sets register 0 to 2 */
    execute_instruction(set_word, &cpu, &memory);

    /* Bit shifts register 0 twice resulting in a value of 8 */
    execute_instruction(bsl_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 8);
}

TEST(CPU, test_bsl_instruction_with_overflow) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    uint32_t set_word = BITMASK_9 | BITMASK_8 | SET_BITMASK;
    uint32_t setu_word = SETU_BITMASK;
    uint32_t bsl_word = BITMASK_16 | BITMASK_11 | BSL_BITMASK;

    /* Using the set and setu words we set the highest and lowest order bit of register 0 to 1 resulting in a value of 
     * -2147483647 */
    execute_instruction(set_word, &cpu, &memory);
    execute_instruction(setu_word, &cpu, &memory);

    /* Bit shifts register 0 once while letting it overflow back onto the lower order bits resulting in a value of 3 */
    execute_instruction(bsl_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 3);
}
