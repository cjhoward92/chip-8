#ifndef __INPUT_H__
#define __INPUT_H__

#include "stdhdr.h"

#define TEST_BIT(n, p) !!(n & (1u << p))

int init_keyboard();

int shutdown_keyboard();

unsigned char get_key();

typedef struct c8_keyboard_t {
    char* path;
    char* name;
    int device_fd;
    struct c8_keyboard_t *next;
} c8_keyboard_t;

#endif