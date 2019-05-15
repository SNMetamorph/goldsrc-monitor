#pragma once
#include <stdint.h>

struct module_info_t
{
	uint8_t *base_addr;
	uint8_t *entry_point_addr;
	size_t image_size;
};