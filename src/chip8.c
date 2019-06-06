#include "chip8.h"

static unsigned short pc = 0x200;
static unsigned short opcode = 0;
static unsigned short i_reg = 0;
static unsigned short sp = 0;

static unsigned char v[REGISTER_COUNT];
static unsigned char memory[TOTAL_MEMORY];
static unsigned char chip8_fontset[FONT_SET];
static unsigned char gfx[GFX_MEMORY];
static unsigned short stack[STACK_COUNT];
static unsigned char key[KEY_COUNT];

static unsigned char delay_timer;
static unsigned char sound_timer;

void chip8_init() {
    printf("INIT CHIP-8\n");

    // Reset registers and static variables
    pc = 0x200;
    opcode = 0;
    i_reg = 0;
    sp = 0;

    // Reset memory
    memset(gfx, 0, GFX_MEMORY);
    memset(stack, 0, STACK_COUNT);
    memset(v, 0, REGISTER_COUNT);
    memset(memory, 0, TOTAL_MEMORY);

    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;
}

void chip8_emulate_cycle() {
    printf("CHIP-8 Emulation\n");
}
