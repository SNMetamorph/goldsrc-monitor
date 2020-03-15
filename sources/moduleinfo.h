#pragma once
#include <stdint.h>

struct moduleinfo_t
{
    uint8_t *baseAddr;
    uint8_t *entryPointAddr;
    size_t imageSize;
};