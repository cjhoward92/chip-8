#include "chip8.h"

/**
 * CHIP-8
 * The CHIP-8 lang only uses up to 4kb of RAM
 * Generally, it starts at 0x200 (512) and ends at
 * 0xFFF (4096). "pc" is the program counter, and the
 * start of RAM.
 * */

// Register values and other primitives
static unsigned short pc = MEMORY_START;
static unsigned short opcode = 0;
static unsigned short i_reg = 0;
static unsigned short sp = 0;

// General arrays (memory and such)
static unsigned char v[REGISTER_COUNT];
static unsigned char memory[TOTAL_MEMORY];
static unsigned char gfx[GFX_MEMORY];
static unsigned short stack[STACK_COUNT];
static unsigned char key[KEY_COUNT];

// TODO: move to header file?
static unsigned char chip8_fontset[FONT_SET] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Timers
static unsigned char delay_timer;
static unsigned char sound_timer;

// Global error handling
static char err_code = 0;

static void reset_gfx_mem() {
    memset(gfx, 0, GFX_MEMORY * sizeof(unsigned char));
}

static void reset_stack() {
    memset(stack, 0, STACK_COUNT * sizeof(unsigned short));
}

static void reset_registers() {
    memset(v, 0, REGISTER_COUNT * sizeof(unsigned char));
}

static void reset_memory() {
    memset(memory, 0, TOTAL_MEMORY * sizeof(unsigned char));
}

// Initializes the CHIP-8 inforamtion.
// Plz call this once, and only once.
// I guess I could add some checks for that...
void chip8_init() {
    printf("INIT CHIP-8\n");

    // Reset registers and static variables
    pc = MEMORY_START;
    opcode = 0;
    i_reg = 0;
    sp = 0;

    // Reset memory
    reset_gfx_mem();
    reset_stack();
    reset_registers();
    reset_memory();

    // Load fontset into memory
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    printf("CHIP-8 INIT COMPLETE\n");
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
    unsigned char *buff = malloc(sizeof(unsigned char) * (TOTAL_MEMORY - MEMORY_START));
    size_t size = fread(buff, 1, TOTAL_MEMORY - MEMORY_START, binary);
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

/* STARTING OPCODES */
// OpCode table found here: https://en.wikipedia.org/wiki/CHIP-8#Opcode_table

static void fail_opcode() {
    err_code = INVALID_OPCODE;
}

// Clears the display
static void op00E0_clear_display() {
    reset_gfx_mem();
}

// Returns from the current subroutine
static void op00EE_return() {
    if (!sp) {
        // Returning when there is nothing to return from...
        // I suppose we can treat this as exit?
        err_code = NO_STACK_TO_UNWIND;
        return;
    }

    // Return the program counter to the
    // last function found on the stack
    pc = stack[--sp];
}

static void exec_0x0000_opcodes() {
    if (!(opcode & 0xFFFF)) {
        err_code = TERMINATE; // THIS IS TERMINATE FOR NOW
        return;
    }

    switch (opcode & 0x000F)
    {
    case 0x0:
        op00E0_clear_display();
        break;
    case 0xE:
        op00EE_return();
        break;
    default:
        fail_opcode();
        break;
    }
}

// GOTO the address at NNN found in the OpCode
static void op1NNN_goto() {
    pc = FETCH_NNN(opcode);
}

// Call the subroutine at NNN found in the OpCode
static void op2NNN_call() {
    if (sp < 0 || sp >= STACK_COUNT) {
        err_code = STACK_OVERFLOW;
        return;
    }

    // Placing the current Program Counter
    // in the stack so we can retrieve it later
    stack[sp++] = pc;
    pc = FETCH_NNN(opcode);
}

// Check the equality of vX and NN
static void op3XNN_eqVXNN() {
    unsigned char reg = FETCH_X(opcode);
    unsigned char num = FETCH_NN(opcode);

    // if vX == NN then skip an instruction
    if (v[reg] == num)
        pc += 2;
}

// Check the inequality of vX and NN
static void op4XNN_neqVXNN() {
    unsigned char reg = FETCH_X(opcode);
    unsigned char num = FETCH_NN(opcode);

    // if vX != NN then skip an instruction
    if (v[reg] != num)
        pc += 2;
}

// Check the equality of vX and vY
static void op5XY0_eqVXVY() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    // if vX == vY then skip the next instruction
    if (v[regx] == v[regy])
        pc += 2;
}

// Set vX to the value NN
static void op6XNN_set() {
    unsigned char reg = FETCH_X(opcode);
    unsigned char num = FETCH_NN(opcode);

    v[reg] = num;
}

// Add the value NN to vX
static void op7XNN_add() {
    unsigned char reg = FETCH_X(opcode);
    unsigned char num = FETCH_NN(opcode);

    v[reg] += num;
}

// Copy the value of vY into vX
static void op8XY0_copy() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    v[regx] = v[regy];
}

// Bitwise OR against vX, vY with result stored in vX
static void op8XY1_or() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    v[regx] |= v[regy];
}

// Bitwise AND against vX, vY with result stored in vX
static void op8XY2_and() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    v[regx] &= v[regy];
}

// Bitwise XOR against vX, vY with result stored in vX
static void op8XY3_xor() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    v[regx] ^= v[regy];
}

// Add vY to vX and set vF if there is a carry
static void op8XY4_add_carry() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    unsigned short result = v[regx] + v[regy];
    if (result & REQUIRES_CARRY)
        v[FLAG_REG] = 1;
    else
        v[FLAG_REG] = 0;
    
    // We only want the lower byte
    v[regx] = (result & 0x00FF);
}

// If vX > vY then set vF = 1, then subtract vY from vX
static void op8XY5_sub_borrow() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    if (v[regx] > v[regy])
        v[FLAG_REG] = 1;
    else
        v[FLAG_REG] = 0;
    
    v[regx] -= v[regy];
}

// Store LSB of vX in vF then shift right
static void op8XY6_shr() {
    unsigned char regx = FETCH_X(opcode);

    v[FLAG_REG] = v[regx] & LSB;
    v[regx] >>= 1;
}

// If vY > vX then set vF = 1, then subtract vX from vY
static void op8XY7_rev_sub_borrow() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    if (v[regy] > v[regx])
        v[FLAG_REG] = 1;
    else
        v[FLAG_REG] = 0;
    
    v[regx] = v[regy] - v[regx];
}

// Store MSB of vX in vF then shift right
static void op8XYE_shl() {
    unsigned char regx = FETCH_X(opcode);

    v[FLAG_REG] = v[regx] & MSB;
    v[regx] <<= 1;
}

static void exec_0x8000_opcodes() {
    switch (opcode & 0xF)
    {
    case 0x0:
        op8XY0_copy();
        break;
    case 0x1:
        op8XY1_or();
        break;
    case 0x2:
        op8XY2_and();
        break;
    case 0x3:
        op8XY3_xor();
        break;
    case 0x4:
        op8XY4_add_carry();
        break;
    case 0x5:
        op8XY5_sub_borrow();
        break;
    case 0x6:
        op8XY6_shr();
        break;
    case 0x7:
        op8XY7_rev_sub_borrow();
        break;
    case 0xE:
        op8XYE_shl();
        break;
    default:
        fail_opcode();
        break;
    }
}

// Check the inequality of vX and vY
static void op9XY0_neqVXVY() {
    unsigned char regx = FETCH_X(opcode);
    unsigned char regy = FETCH_Y(opcode);

    // if vX != vY then skip the next instruction
    if (v[regx] != v[regy])
        pc += 2;
}

// Load NNN into I
static void opANNN_load() {
    i_reg = FETCH_NNN(opcode);
}

// JP v0 - Jump to the value NNN + v0
static void opBNNN_jumpv0() {
    unsigned short addr = FETCH_NNN(opcode);
    pc = addr + v[0];
}

// set vX = RND & NN
static void opCXNN_rnd() {
    unsigned char x = FETCH_X(opcode);
    unsigned short num = FETCH_NN(opcode);
    unsigned short rnd = rand() & 0xFF;

    v[x] = rnd & num;
}

static void exec_next_opcode() {
    // Let's first get the OpCode
    opcode = FETCHOPCODE(memory, pc);
    
    // Immediately "execute" the OpCode
    // so we can progress the program counter
    // without worrying about it being
    // incremented later...
    EXECOPCODE(i_reg, opcode, pc);

    switch (opcode & 0xF000)
    {
    case 0x0000:
        exec_0x0000_opcodes();
        break;
    case 0x1000:
        op1NNN_goto();
        break;
    case 0x2000:
        op2NNN_call();
        break;
    case 0x3000:
        op3XNN_eqVXNN();
        break;
    case 0x4000:
        op4XNN_neqVXNN();
        break;
    case 0x5000:
        op5XY0_eqVXVY();
        break;
    case 0x6000:
        op6XNN_set();
        break;
    case 0x7000:
        op7XNN_add();
        break;
    case 0x8000:
        exec_0x8000_opcodes();
        break;
    case 0x9000:
        op9XY0_neqVXVY();
        break;
    case 0xA000:
        opANNN_load();
        break;
    case 0xB000:
        opBNNN_jumpv0();
        break;
    case 0xC000:
        opCXNN_rnd();
        break;
    case 0xD000:
        break;
    case 0xE000:
        break;
    case 0xF000:
        break;
    default:
        fail_opcode();
        break;
    }
    return;
}

// This is supposed to emulate the
// CHIP-8 clock cycle. We will see
// how well it actually works out...
void chip8_emulate_cycle() {
    printf("CHIP-8 Emulation\n");

    for(;;)
    {
        exec_next_opcode();
        if (err_code) {
            // TODO: print general error message?
            return;
        }

        // Do timer stuff...
    }

    // Free memory...
}