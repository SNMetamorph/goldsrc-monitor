#pragma once
#include "moduleinfo.h"
#include <stdint.h>
#include <Windows.h>


HMODULE FindModuleByExport(HANDLE procHandle, const char *exportName);
bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo);
void *FindMemoryInt32(void *startAddr, void *endAddr, uint32_t value);
void *FindPatternAddress(void *startAddr, void *endAddr, const char *pattern, const char *mask);
