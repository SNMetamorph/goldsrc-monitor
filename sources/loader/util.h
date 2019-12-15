#pragma once
#include "module_info.h"
#include <Windows.h>

int FindProcessID(const wchar_t *process_name);
HMODULE FindProcessModule(HANDLE proc_handle, const wchar_t *module_name);
size_t GetFunctionOffset(HMODULE module_handle, const char *func_name);
bool GetModuleInfo(HANDLE proc_handle, HMODULE module_handle, module_info_t &module_info);