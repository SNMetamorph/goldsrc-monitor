#include "util.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <stdint.h>

int find_process(const wchar_t *process_name)
{
	HANDLE			process_snap;
	PROCESSENTRY32	process_entry;

	process_entry.dwSize	= sizeof(process_entry);
	process_snap			= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (process_snap == INVALID_HANDLE_VALUE) 
		return NULL;
	
	if (!Process32First(process_snap, &process_entry)) {
		CloseHandle(process_snap);
		return NULL;
	}

	do {
		if (wcscmp(process_name, process_entry.szExeFile) == 0) 
			return process_entry.th32ProcessID;
	} 
	while (Process32Next(process_snap, &process_entry));
	CloseHandle(process_snap);
	return NULL;
}

HMODULE find_process_module(HANDLE proc_handle, const wchar_t *target_name)
{
	DWORD	buffer_size;
	HMODULE module_stub;
	HMODULE module_result = NULL;
	wchar_t	module_path[MAX_PATH];

	if (EnumProcessModules(proc_handle, &module_stub, sizeof(module_stub), &buffer_size))
	{
		size_t	modules_count	= buffer_size / sizeof(HMODULE);
		HMODULE	*modules		= new HMODULE[modules_count];
		EnumProcessModules(proc_handle, modules, buffer_size, &buffer_size);

		for (size_t i = 0; i < modules_count; ++i)
		{
			size_t chars_count = sizeof(module_path) / sizeof(module_path[0]);
			GetModuleFileNameEx(proc_handle, modules[i], module_path, chars_count);
			wchar_t *module_name = wcsrchr(module_path, '\\') + 1;
			if (_wcsicmp(module_name, target_name) == 0)
			{
				module_result = modules[i];
				break;
			}
		}
		delete[] modules;
	}
	return module_result;
}

size_t get_function_offset(HMODULE module_handle, const char *func_name)
{
	uint8_t *func_addr = (uint8_t *)GetProcAddress(module_handle, func_name);
	return (size_t)(func_addr - (uint8_t*)module_handle);
}

bool get_module_info(HANDLE proc_handle, HMODULE module_handle, module_info_t &module_info)
{
	MODULEINFO minfo;
	if (!GetModuleInformation(proc_handle, module_handle, &minfo, sizeof(minfo)))
		return false;

	module_info.base_addr			= (uint8_t*)minfo.lpBaseOfDll;
	module_info.image_size			= minfo.SizeOfImage;
	module_info.entry_point_addr	= (uint8_t*)minfo.EntryPoint;
	return true;
}
