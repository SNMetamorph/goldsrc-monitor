#pragma once
#include <stdint.h>

struct module_info_t
{
	uint8_t *baseAddr;
	uint8_t *entryPointAddr;
	size_t imageSize;
};