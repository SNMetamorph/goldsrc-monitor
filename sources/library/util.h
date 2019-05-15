#pragma once
#include "module_info.h"
#include <stdint.h>
#include <Windows.h>

int get_build_number();
bool get_module_info(HANDLE proc_handle, HMODULE module_handle, module_info_t &module_info);
void *find_memory_value(uint32_t *start_addr, size_t scan_len, uint32_t value);
void find_server_module(HMODULE & module_handle);
void *find_pattern_address(uint8_t *start_addr, size_t scan_len, const char *pattern, const char *mask);

