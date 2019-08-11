#include "chip8.h"

static void test() {
    // unsigned short opcode = 0xAFFF;
    // unsigned short first = opcode & 0xF000;
    // printf("%u\n", first);
    unsigned char x = 255;
    unsigned char y = 10;
    x += y;
    printf("%hhu\n", x);
}

int main() {
    // chip8_init();
    // if (chip8_load_binary("/media/carson/ExtraDrive1/Development/CHIP-8/test/test_binary") < 0) {
    //     return -1;
    // }

    test();

    printf("Hello, this is the chip8 emulator\n\n");
    return 0;
}