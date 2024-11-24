#ifndef CPU_H_
#define CPU_H_

extern char main_memory[];
extern int PC;
extern int registers[8];

char get_op_code(int word);
void execute_instruction(int word);

void execute_set_instruction(int word, int registers[]);
void execute_setu_instruction(int word, int registers[]);
void execute_add_instruction(int word, int registers[]);
void execute_sub_instruction(int word, int registers[]);
void execute_mul_instruction(int word, int registers[]);
void execute_div_instruction(int word, int registers[]);
void execute_mod_instruction(int word, int registers[]);
void execute_and_instruction(int word, int registers[]);
void execute_or_instruction(int word, int registers[]);
void execute_xor_instruction(int word, int registers[]);
void execute_bsr_instruction(int word, int registers[]);
void execute_bsl_instruction(int word, int registers[]);
void execute_ld_instruction(int word, int registers[]);
void execute_put_instruction(int word, int registers[]);
void execute_jmp_instruction(int word, int registers[]);

#endif
