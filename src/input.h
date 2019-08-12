#ifndef __INPUT_H__
#define __INPUT_H__

#include "stdhdr.h"

#define TEST_BIT(n, p) !!(n & (1u << p))

int init_keyboard();

unsigned char get_key();

#endif