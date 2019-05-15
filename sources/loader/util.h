#pragma once
#include <Windows.h>
#include "module_info.h"

int find_process(const wchar_t *process_name);
HMODULE find_process_module(HANDLE proc_handle, const wchar_t *module_name);
size_t get_function_offset(HMODULE module_handle, const char *func_name);
bool get_module_info(HANDLE proc_handle, HMODULE module_handle, module_info_t &module_info);