extern "C" {
    #include "../src/cpu.h"
    #include "../src/memory.h"
    #include "../src/bit_macros.h"
}
#include <gtest/gtest.h>


TEST(CPU, use_jmp_instruction_to_set_program_counter) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = 0b00000000000000000001000000010000;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 1);
}

TEST(CPU, jmp_skip_instruction_using_control_bit) {
    CPU cpu = init_cpu();
    Memory memory = init_memory();

    int word = 0b00000000000000000001000100010000;
    execute_instruction(word, &cpu, &memory);
    EXPECT_EQ(cpu.program_counter, 0);
}
