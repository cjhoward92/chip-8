#include "input.h"
#include "chip8.h"

// static void test() {
//     // unsigned short opcode = 0xAFFF;
//     // unsigned short first = opcode & 0xF000;
//     // printf("%u\n", first);
//     unsigned char x = 255;
//     unsigned char y = 10;
//     x += y;
//     printf("%hhu\n", x);
// }

int main() {
    chip8_init();
    if (chip8_load_binary("/media/carson/ExtraDrive1/Development/CHIP-8/test/set_add.chip") < 0) {
        return -1;
    }

    init_keyboard();

    // chip8_run();

    // test();

    printf("emulation complete\n\n");
    return 0;
}