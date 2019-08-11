#include "chip8.h"

int square(int num) {
    unsigned short opcode = 0xAFFF;
    unsigned short first = opcode & 0xF000;
    printf("%u\n", first);
    return 0;
}

int main() {
    // chip8_init();
    // if (chip8_load_binary("/media/carson/ExtraDrive1/Development/CHIP-8/test/test_binary") < 0) {
    //     return -1;
    // }

    square(0);

    printf("Hello, this is the chip8 emulator\n\n");
    return 0;
}