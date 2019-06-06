#ifndef __CHIP_8__
#define __CHIP_8__

#include "stdhdr.h"

// Globals
#define TOTAL_MEMORY 4096
#define GFX_MEMORY 64 * 32
#define REGISTER_COUNT 16
#define STACK_COUNT 16
#define KEY_COUNT 16

typedef unsigned short op_code;
typedef unsigned char memory;
typedef unsigned char gfx_memory;

typedef unsigned short i_reg;
typedef unsigned short pc_reg;
typedef unsigned char v_reg;

typedef unsigned char delay_timer;
typedef unsigned char sound_timer;

typedef unsigned short stk_ptr;
typedef unsigned short stack;

typedef unsigned char key;

// Fetch the opcode at the specified memory address
#define FETCHOPCODE(mem, pc) mem[pc] << 8 | mem[pc + 1]

// Execute the opcode
#define EXEC_MASK 0x0fff
#define EXECOPCODE(reg, op, pc) {\
    reg = op & EXEC_MASK;\
    pc += 2;\
}

void chip8_init();

void chip8_emulate_cycle();

#endif