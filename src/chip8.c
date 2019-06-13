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
    memset(gfx, 0, GFX_MEMORY * sizeof(unsigned char));
    memset(stack, 0, STACK_COUNT * sizeof(unsigned short));
    memset(v, 0, REGISTER_COUNT * sizeof(unsigned char));
    memset(memory, 0, TOTAL_MEMORY * sizeof(unsigned char));

    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    printf("CHIP-8 INIT COMPLETE\n");
}

void chip8_emulate_cycle() {
    printf("CHIP-8 Emulation\n");
}

int chip8_load_binary(const char *path) {
    if (!path) {
        fprintf(stderr, "A path is required\n");
        return -1;
    }

    FILE *binary = fopen(path, "rb");
    printf("Read binary at '%s'\n", path);

    // Allocate the file to a new buffer
    unsigned char *buff = malloc(sizeof(unsigned char) * TOTAL_MEMORY);
    size_t size = fread(buff, 1, TOTAL_MEMORY, binary);
    if (size <= 0) {
        fprintf(stderr, "Could not read binary\n");
        return -1;
    }
    printf("Size of binary is %ld B\n", size);

    for (int i = 0; i < size; ++i)
        memory[i + (int)pc] = buff[i];
    
    free(buff);
    fclose(binary);

    printf("CHIP-8 BINARY LOADED\n");
    return 0;
}
