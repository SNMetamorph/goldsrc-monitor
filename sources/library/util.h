#pragma once
#include "moduleinfo.h"
#include <stdint.h>
#include <Windows.h>

bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo);
void *FindMemoryInt32(void *startAddr, size_t scanLen, uint32_t value);
void FindServerModule(HMODULE &moduleHandle);
void *FindPatternAddress(void *startAddr, int scanLen, const char *pattern, const char *mask);
