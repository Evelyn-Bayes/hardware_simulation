extern "C" {
    #include "../src/cpu.h"
    #include "../src/memory.h"
    #include "../src/bit_utils.h"
}
#include <gtest/gtest.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> JMP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_jmp_intruction_with_fixed_value_offset) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BITMASK_13 | BITMASK_5 | JMP_BITMASK;
    // Set the program counter to 1
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_jmp_instruction_skips_using_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BITMASK_13 | BITMASK_9 | BITMASK_5 | JMP_BITMASK;

    /* Attempts to set the program counter to 1 but skips because control bit is set and control register is 0 */
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 0);
}

TEST(CPU, test_jmp_instruction_doesnt_skip_using_control_bit_and_positive_control_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    int jmp_word = BITMASK_13 | BITMASK_10 | BITMASK_9 | BITMASK_5 | JMP_BITMASK;

    /* Sets register 1 to 1 */
    execute_instruction(set_word, &cpu, &memory);
    /* Attempts to set the program counter to 1 and succeeds because control bit is set and control register is 1 */
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_jmp_instruction_works_with_base_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BITMASK_10 | SET_BITMASK;
    int jmp_word = BITMASK_13 | BITMASK_5 | JMP_BITMASK;

    /* Sets register 0 to 2 */
    execute_instruction(set_word, &cpu, &memory);
    /* Sets program counter to the value of register 0 plus an offset of 1 */
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 3);
}

TEST(CPU, test_jmp_instruction_works_with_offset_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    int jmp_word = BITMASK_13 | JMP_BITMASK;

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

    int set_word = BITMASK_9 | BITMASK_5 | SET_BITMASK;
    int put_word = BITMASK_8 | PUT_BITMASK;

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

    int word = BITMASK_8 | SETU_BITMASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 16777216);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ADD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SUB >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MUL >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DIV >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> MOD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> AND >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_and_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 00000011 */
    int set_word_1 = BITMASK_10 | BITMASK_9 | SET_BITMASK;

    /* Sets the lower byte of register 1 to 00000101 */
    int set_word_2 = BITMASK_11 | BITMASK_9 | BITMASK_5 | SET_BITMASK;

    /* Performs an AND operation on register 0 and register 1 */
    int and_word = BITMASK_11 | AND_BITMASK;

    execute_instruction(set_word_1, &cpu, &memory);
    execute_instruction(set_word_2, &cpu, &memory);
    execute_instruction(and_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_and_instruction_with_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    /* Sets the lower byte of register 0 to 00000110 */
    int set_word = BITMASK_11 | BITMASK_10 | SET_BITMASK;

    /* Performs an AND operation on register 0 and value 101 */
    int and_word = BITMASK_17 | BITMASK_15 | BITMASK_14 | AND_BITMASK;

    execute_instruction(set_word, &cpu, &memory);
    execute_instruction(and_word, &cpu, &memory);

    EXPECT_EQ(cpu.registers[0], 4);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> OR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> XOR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BSR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

TEST(CPU, test_bsr_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BITMASK_11 | SET_BITMASK;
    int bsr_word = BITMASK_12 | BSR_BITMASK;

    /* Sets register 0 to 4 */
    execute_instruction(set_word, &cpu, &memory);

    /* Bit shifts register 0 twice resulting in a value of 1 */
    execute_instruction(bsr_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_bsr_instruction_with_overflow) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BITMASK_11 | BITMASK_10 | BITMASK_9 | SET_BITMASK;
    int bsr_word = BITMASK_16 | BITMASK_12 | BSR_BITMASK;

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

    int set_word = BITMASK_10 | SET_BITMASK;
    int bsl_word = BITMASK_12 | BSL_BITMASK;

    /* Sets register 0 to 2 */
    execute_instruction(set_word, &cpu, &memory);

    /* Bit shifts register 0 twice resulting in a value of 8 */
    execute_instruction(bsl_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 8);
}

TEST(CPU, test_bsl_instruction_with_overflow) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BITMASK_9 | BITMASK_8 | SET_BITMASK;
    int setu_word = SETU_BITMASK;
    int bsl_word = BITMASK_16 | BITMASK_11 | BSL_BITMASK;

    /* Using the set and setu words we set the highest and lowest order bit of register 0 to 1 resulting in a value of 
     * -2147483647 */
    execute_instruction(set_word, &cpu, &memory);
    execute_instruction(setu_word, &cpu, &memory);

    /* Bit shifts register 0 once while letting it overflow back onto the lower order bits resulting in a value of 3 */
    execute_instruction(bsl_word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 3);
}
