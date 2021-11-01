#ifndef global_h
#define global_h

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>

namespace globals {

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
} // namespace globals

using Byte = uint8_t;
using Word = uint16_t;
using u32 = unsigned int;

static constexpr u32 MAX_MEM = 4096;

#endif