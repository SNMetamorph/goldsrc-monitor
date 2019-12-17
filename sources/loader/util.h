#pragma once
#include "moduleinfo.h"
#include <Windows.h>

int FindProcessID(const wchar_t *process_name);
HMODULE FindProcessModule(HANDLE proc_handle, const wchar_t *module_name);
size_t GetFunctionOffset(HMODULE module_handle, const char *func_name);
bool GetModuleInfo(HANDLE proc_handle, HMODULE module_handle, moduleinfo_t &module_info);