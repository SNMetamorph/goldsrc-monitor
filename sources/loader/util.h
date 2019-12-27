#pragma once
#include "moduleinfo.h"
#include <Windows.h>

int FindProcessID(const wchar_t *processName);
HMODULE FindProcessModule(HANDLE procHandle, const wchar_t *moduleName);
size_t GetFunctionOffset(HMODULE moduleHandle, const char *funcName);
bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo);