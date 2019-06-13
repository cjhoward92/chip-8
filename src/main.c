#include "chip8.h"

int main() {
    chip8_init();
    if (chip8_load_binary("/media/carson/ExtraDrive1/Development/CHIP-8/test/test_binary") < 0) {
        return -1;
    }

    printf("Hello, this is the chip8 emulator\n\n");
    return 0;
}