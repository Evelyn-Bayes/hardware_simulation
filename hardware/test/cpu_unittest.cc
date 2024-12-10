extern "C" {
    #include "../src/cpu.h"
    #include "../src/memory.h"
    #include "../src/bit_macros.h"
}
#include <gtest/gtest.h>


TEST(CPU, test_jmp_intruction_with_fixed_value_offset) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BIT_MASK_13 | BIT_MASK_5 | JMP_BIT_MASK;
    // Set the program counter to 1
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_jmp_instruction_skips_using_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BIT_MASK_13 | BIT_MASK_9 | BIT_MASK_5 | JMP_BIT_MASK;

    // Attempts to set the program counter to 1 but skips because control bit is set and control register is 0
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 0);
}

TEST(CPU, test_jmp_instruction_doesnt_skip_using_control_bit_and_positive_control_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BIT_MASK_9 | BIT_MASK_5 | SET_BIT_MASK;
    int jmp_word = BIT_MASK_13 | BIT_MASK_10 | BIT_MASK_9 | BIT_MASK_5 | JMP_BIT_MASK;

    // Sets register 1 to 1
    execute_instruction(set_word, &cpu, &memory);
    // Attempts to set the program counter to 1 and succeeds because control bit is set and control register is 1
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_jmp_instruction_works_with_base_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BIT_MASK_10 | SET_BIT_MASK;
    int jmp_word = BIT_MASK_13 | BIT_MASK_5 | JMP_BIT_MASK;

    // Sets register 0 to 2
    execute_instruction(set_word, &cpu, &memory);
    // Sets program counter to the value of register 0 plus an offset of 1
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 3);
}

TEST(CPU, test_jmp_instruction_works_with_offset_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int set_word = BIT_MASK_9 | BIT_MASK_5 | SET_BIT_MASK;
    int jmp_word = BIT_MASK_13 | JMP_BIT_MASK;

    // Sets register 1 to 1
    execute_instruction(set_word, &cpu, &memory);
    // Sets program counter to the value of 1 using a 0 base register plus a 1 offset register
    execute_instruction(jmp_word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, test_set_instruction) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BIT_MASK_9 | SET_BIT_MASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], 1);
}

TEST(CPU, test_set_instruction_with_negative_value) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BIT_MASK_9 | BIT_MASK_8 | SET_BIT_MASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[0], -16777215);
}

TEST(CPU, test_set_instruction_with_different_register) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = BIT_MASK_9 | BIT_MASK_5 | SET_BIT_MASK;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.registers[1], 1);
}
