#ifndef __CHIP_8__
#define __CHIP_8__

#include "stdhdr.h"

// Globals
#define TOTAL_MEMORY 0xFFF // 4096
#define MEMORY_START 0x200 // 512
#define GFX_MEMORY 64 * 32
#define REGISTER_COUNT 0x10 // 16
#define STACK_COUNT 0x10 // 16
#define KEY_COUNT 0x10 // 16
#define FONT_SET 0x50 // 80

// Fetch the opcode at the specified memory address
#define FETCHOPCODE(mem, pc) mem[pc] << 8 | mem[pc + 1]

// Execute the opcode
#define EXEC_MASK 0x0fff
#define EXECOPCODE(reg, op, pc) {\
    reg = op & EXEC_MASK;\
    pc += 2;\
}

enum ERR_CODES {
    INVALID_OPCODE = -1,
    NO_STACK_TO_UNWIND = -2,
    STACK_OVERFLOW = -3,    
};

void chip8_init();

void chip8_emulate_cycle();

int chip8_load_binary(const char *path);

#endif