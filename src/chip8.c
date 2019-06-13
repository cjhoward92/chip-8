#include "chip8.h"

// Register values and other primitives
static unsigned short pc = 0x200;
static unsigned short opcode = 0;
static unsigned short i_reg = 0;
static unsigned short sp = 0;

// General arrays (memory and such)
static unsigned char v[REGISTER_COUNT];
static unsigned char memory[TOTAL_MEMORY];
static unsigned char chip8_fontset[FONT_SET];
static unsigned char gfx[GFX_MEMORY];
static unsigned short stack[STACK_COUNT];
static unsigned char key[KEY_COUNT];

// Timers
static unsigned char delay_timer;
static unsigned char sound_timer;

// Initializes the CHIP-8 inforamtion.
// Plz call this once, and only once.
// I guess I could add some checks for that...
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

// This is supposed to emulate the
// CHIP-8 clock cycle. We will see
// how well it actually works out...
void chip8_emulate_cycle() {
    printf("CHIP-8 Emulation\n");
}

// Loads the specified binary into memory.
// Right now this is constrained at
// TOTAL_MEMEORY - 0x200 (512) bytes,
// or 3584 bytes.
int chip8_load_binary(const char *path) {
    // Ain't no file, ain't my problem
    if (!path) {
        fprintf(stderr, "A path is required\n");
        return -1;
    }

    // Let's open the file
    FILE *binary = fopen(path, "rb");
    printf("Reading binary at '%s'\n", path);

    // Allocate the file to a new buffer
    unsigned char *buff = malloc(sizeof(unsigned char) * TOTAL_MEMORY);
    size_t size = fread(buff, 1, TOTAL_MEMORY, binary);
    if (size <= 0) {
        fprintf(stderr, "Could not read binary\n");
        return -1;
    }
    printf("Size of binary is %ld B\n", size);

    // Copy binary to memory
    for (int i = 0; i < size; ++i)
        memory[i + (int)pc] = buff[i];
    
    // Cleanup time
    free(buff);
    fclose(binary);

    printf("CHIP-8 BINARY LOADED\n");
    return 0;
}
