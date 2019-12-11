#pragma once
#include "module_info.h"
#include <stdint.h>
#include <Windows.h>

int GetBuildNumber();
bool GetModuleInfo(HANDLE proc_handle, HMODULE module_handle, module_info_t &module_info);
void *FindMemoryValue(uint32_t *start_addr, size_t scan_len, uint32_t value);
void FindServerModule(HMODULE & module_handle);
void *FindPatternAddress(uint8_t *start_addr, size_t scan_len, const char *pattern, const char *mask);

