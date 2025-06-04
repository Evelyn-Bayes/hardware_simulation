## Registers                                                 

The program counter (PC) is a 32-bit register that holds the address of the next instruction to execute. All general purpose registers (R1-R8) are also 32 bits and can be used to store arbitrary values during program execution.

 Register | Purpose                  
----------|:-------------------------
 PC       | Program Counter
 R1       | General Purpose Register
 R2       | General Purpose Register
 R3       | General Purpose Register
 R4       | General Purpose Register
 R5       | General Purpose Register  
 R6       | General Purpose Register  
 R7       | General Purpose Register
 R8       | General Purpose Register

## Memory

This CPU architecture uses a byte-addressable memory model.

When reading or writing to memory the operation must be aligned or within the systems word size (32 bit), so:
 - A 4-byte operation can only be performed on addresses that are multiples of 4
 - A 2-byte operation can only be performed on addresses that are multiples of 2
 - A 1-byte operation can be performed on any address

#### Address Ranges

 Name | Size | Address Range
------|:-----|:-----------------------
 RAM  | 1MB  | 0x00000000 - 0x000FFFFF 

## Instruction Set

This section defines the instruction set for a CPU using a 32-bit word size, using Little Endian format.
The instruction set is intended to be simple and easy to implement.

From this point, we refer to bits in the following manner:
 1. The least significant bit (right-most) is bit 1
 2. The most significant bit (left-most) is bit 32

Each instruction is documented with its bit layout and corresponding mnemonic.

### Summary of instructions

 Code | Name                        | Description            
:-----|:----------------------------|:------------------------
 000  | JMP                         | Control flow
 001  | ST / LD                     | Memory access
 010  | SET                         | Set lower bits
 011  | SETU                        | Set upper bits
 100  | ADD / SUB / MUL / DIV / MOD | Arithmetic operations
 101  | AND / OR / XOR              | Bitwise operations
 110  | BSR / BSL                   | Bit shifts and rotates
 111  | Unused                      | Unused

### JMP

Sets the program counter to the value of the base register plus an offset.

 Bit layout   ||
:-------------|:------------------------------------------------------------------------------------------------------
 Bits 1 - 3   | Op Code - 000
 Bit  4       | Control bit - when set, bits 12–32 are treated as an immediate offset instead of a register reference
 Bit  5       | Control bit - when set, the control register determines whether the instruction is executed
 Bits 6 - 8   | Base register
 Bits 9 - 11  | Control register
 Bits 12 - 32 | Offset register
 
 Mnemonic                                                    ||
:------------------------------------------------------------|----------------------------------------
 JMP   <base register> <offset register>                     | Unconditional jump
 JMPI  <base register> <immediate offset>                    | Jump with immediate offset
 JMPC  <base register> <offset register>  <control register> | Conditional jump
 JMPIC <base register> <immediate offset> <control register> | Conditional jump with immediate offset

**Note**: When control bit 4 is set, bits 12–32 serve as an immediate offset, enabling direct jumps within a ~1GB address space (2^20 × 4 × 256 − 1) without first loading the offset into a register. This improves efficiency in medium-sized programs. The use of base and offset registers also supports flexible memory layouts and linking within larger program contexts.

### ST / LD

Transfers data between a register and a memory address specified by the base register plus offset.

 Bit layout   ||
:-------------|----------------------------------------------------------------------------------------------------
 Bits 1 - 3   | Op Code - 001
 Bit  4       | Control bit - selects operation (0 = store, 1 = load)
 Bits 5 - 6   | Control bits - specifies byte width to transfer (0 = 1 byte, 1 = 2 bytes, 2 = 4 bytes, 3 = unused)
 Bit  7       | Control bit - when set, bits 14-32 are treated as an immediate offset
 Bits 8 - 10  | Source / destination register
 Bits 11 - 13 | Base address register
 Bits 14 - 32 | Offset register or immediate value

 Mnemonic                                      ||
:----------------------------------------------|----------------------------------------
 STB  <src>  <base register> <offset register> | Store byte
 STH  <src>  <base register> <offset register> | Store half-word
 STW  <src>  <base register> <offset register> | Store word
 STBI <src>  <base register> <offset>          | Store byte using immediate offset
 STHI <src>  <base register> <offset>          | Store half-word using immediate offset
 STWI <src>  <base register> <offset>          | Store word using immediate offset
 LDB  <dest> <base register> <offset register> | Load byte
 LDH  <dest> <base register> <offset register> | Load half-word
 LDW  <dest> <base register> <offset register> | Load word
 LDBI <dest> <base register> <offset>          | Load byte using immediate offset
 LDHI <dest> <base register> <offset>          | Load half-word using immediate offset
 LDWI <dest> <base register> <offset>          | Load word using immediate offset

**Note**: When control bit 7 is set, bits 14–32 represent an immediate offset, allowing access to up to ~64MB of memory (2^18 x 256 − 1) without a separate instruction to set the offset. Using base and offset registers supports flexible memory mapping, such as paged memory regions. For partial-word loads (1 or 2 bytes), the upper bits of the destination register are zero-filled.

### SET

Sets a register's value.

 Bit layout  ||
:------------|-------------------------------------------------------------------------------------------------
 Bits 1 - 3  | Op Code - 010
 Bit  4      | Control bit - when set, the upper bits (26–32) are filled with 1s; otherwise, they are set to 0
 Bits 5 - 7  | Destination register
 Bits 8 - 32 | Value (lower 25 bits)

 Mnemonic            ||
:--------------------|--------------------
 SET  <dest> <value> | Set positive value
 SETN <dest> <value> | Set negative value

**Note**: The combination of the control bit and 25-bit value allows encoding signed values in the range −33,554,432 to 33,554,431.

### SETU

Sets the upper bits (26 - 32) of a register.

 Bit layout   ||
--------------|----------------------
 Bits 1 - 3   | Op Code - 011
 Bits 4 - 6   | Destination register
 Bits 7 - 12  | Value for bits 26-31
 Bits 15 - 32 | Unused

Mnemonic                            ||
:-----------------------------------|--------------------------------------------------
SETU <destination register> <value> | Set upper bits

**Note**: Used with the SET instruction to assign a full 32-bit value in two steps.

### ADD / SUB / MUL / DIV / MOD

Performs arithmetic operation between two operands and stores the result in a destination register.

 Bit layout   ||
:-------------|-----------------------------------------------------------------------------------------------
 Bits 1 - 3   | Op Code - 100
 Bits 4 - 6   | Control bits - selects operation (0 = add, 1 = subtract, 2 = multiply, 3 = divide, 4 = modulo)
 Bit  7       | Control bit - when set, bits 14-32 are treated as an immediate value
 Bits 8 - 10  | Destination register
 Bits 11 - 13 | First source register
 Bits 14 - 32 | Second source register or immediate value

 Mnemonic                   ||
:---------------------------|------------------------------------
 ADD  <dest> <src1> <src2>  | Add
 ADDI <dest> <src>  <value> | Add with immediate value
 SUB  <dest> <src1> <src2>  | Subtract
 SUBI <dest> <src>  <value> | Subtract with immediate value
 MUL  <dest> <src1> <src2>  | Multiply
 MULI <dest> <src>  <value> | Multiply with immediate value
 DIV  <dest> <src1> <src2>  | Divide register 1 by register 2
 DIVI <dest> <src>  <value> | Divide register by immediate value
 MOD  <dest> <src1> <src2>  | Modulo register 1 by register 2
 MODI <dest> <src>  <value> | Modulo register by immediate value

**Note**: When control bit 7 is set, bits 14–32 represent an immediate value, allowing operations between a register and a constant up to 524,287 (2^18 − 1), without first loading the value into another register.

### AND / OR / XOR

Performs a bitwise AND, OR, or XOR between two operands and stores the result in a destination register.

 Bit layout   ||
:-------------|------------------------------------------------------------------------------------------------------------------------------------------
 Bits 1 - 3   | Op Code - 101
 Bits 4 - 5   | Control bits - selects operation (0 = AND, 1 = OR, 2 = XOR)
 Bit  6       | Control bit - if set to 1 bits 14 - 32 are treated as an immediate value to perform the operation with
 Bit  7       | Control bit - if set to 1 in combination with control bit 6 the unset bits of the fixed value are set to 1, otherwise they are set to 0
 Bits 8 - 10  | Destination register
 Bits 11 - 13 | First source register
 Bits 14 - 32 | Second source register or immediate value

 Mnemonic                   ||
:---------------------------|-------------------------------------------------------
 AND  <dest> <src1> <src2>  | Bitwise AND
 ANDI <dest> <src>  <value> | Bitwise AND with immediate value
 ANDF <dest> <src>  <value> | Bitwise AND with immediate value, upper bits set to 1
 OR   <dest> <src1> <src2>  | Bitwise OR
 ORI  <dest> <src>  <value> | Bitwise OR with immediate value
 ORF  <dest> <src>  <value> | Bitwise OR with immediate value, upper bits set to 1
 XOR  <dest> <src1> <src2>  | Bitwise XOR
 XORI <dest> <src>  <value> | Bitwise XOR with immediate value
 XORF <dest> <src>  <value> | Bitwise XOR with immediate value, upper bits set to 1

**Note**: When control bit 6 is set, bits 14–32 represent an immediate value, allowing operations between a register and a constant up to 524,287 (2^18 − 1), without first loading the value into another register. When combined with control bit 7 common bit patterns such as 0xFFFFFFFF can be achieved for the immediate value without needing additional instructions to load the value into the second source register.

### BSR / BSL

Performs a bitwise shift on a register by an operand and stores the result in a destination register.

 Bit layout   ||
:-------------|------------------------------------------------------------------------------------
 Bits 1 - 3   | Op Code - 110
 Bit  4       | Control bit - shift direction (0 = right, 1 = left)
 Bit  5       | Control bit - if set to 1 bits 13 - 17 are treated as an immediate value to perform the operation with
 Bit  6       | Control bit - when set, performs a rotate instead of zero-filling shifted-out bits
 Bits 7 - 9   | Destination register
 Bits 10 - 12 | First source register
 Bits 13 - 17 | Second source register or immediate value
 Bits 18 - 32 | Unused

 Mnemonic                           ||
------------------------------------|------------------------------------------------------
 BSR   <dest> <src> <number of bits> | Shift right, zero-fill
 BSRI  <dest> <src> <value>          | Shift right with immediate value, zero-fill
 BSRR  <dest> <src> <number of bits> | Shift right with rotation (lower bits wrap to upper)
 BSRRI <dest> <src> <value>          | Shift right with immediate value and rotation
 BSL   <dest> <src> <number of bits> | Shift left, zero-fill
 BSLI  <dest> <src> <value>          | Shift left with immediate value, zero-fill
 BSLR  <dest> <src> <number of bits> | Shift left with rotation (upper bits wrap to lower)
 BSLRI <dest> <src> <value>          | Shift left with immediate value and rotation
