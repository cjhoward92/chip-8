#ifndef __CHIP_8__
#define __CHIP_8__

#include "stdhdr.h"

// Globals
#define TOTAL_MEMORY 0xFFF // 4095
#define MEMORY_START 0x200 // 512
#define GFX_MEMORY 64 * 32
#define REGISTER_COUNT 0x10 // 16
#define STACK_COUNT 0x10 // 16
#define KEY_COUNT 0x10 // 16
#define FONT_SET 0x50 // 80

#define REQUIRES_CARRY 0x0100
#define FLAG_REG 0xF // 15
#define LSB 0x1 // 1 -> 00000001
#define MSB 0x80 // 128 -> 10000000

// Fetch the opcode at the specified memory address
#define FETCHOPCODE(mem, pc) mem[pc] << 8 | mem[pc + 1]

// Execute the opcode
#define EXEC_MASK 0x0FFF

// This is currently unused and is
// an artifact of misunderstanding...
// I might want to delete it...
#define EXECOPCODE(reg, op, pc) {\
    reg = op & EXEC_MASK;\
    pc += 2;\
}

// Get F in 0x0F00
#define FETCH_X(op) (op & 0x0F00) >> 8

// Get F in 0x00F0
#define FETCH_Y(op) (op & 0x00F0) >> 4

// Get FF in 0x00FF
#define FETCH_NN(op) (op & 0x00FF)

// Get FFF in 0x0FFF
#define FETCH_NNN(op) (op & 0x0FFF)

enum ERR_CODES {
    INVALID_OPCODE = -1,
    NO_STACK_TO_UNWIND = -2,
    STACK_OVERFLOW = -3,
    TERMINATE = 1, 
};

void chip8_init();

void chip8_emulate_cycle();

int chip8_load_binary(const char *path);

#endif