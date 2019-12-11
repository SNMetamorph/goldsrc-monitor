#include "util.h"
#include "module_info.h"

#include <cstring>
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>

// export from dllmain.cpp
extern int (*pfnGetBuildNumber)();

int GetBuildNumber()
{
	if (pfnGetBuildNumber)
		return pfnGetBuildNumber();
	else
		return 0;
}

void *FindPatternAddress(uint8_t *start_addr, size_t scan_len, const char *pattern, const char *mask)
{
	size_t mask_len		= strlen(mask);
	uint8_t *end_addr	= (start_addr + scan_len) - mask_len;
	for (uint8_t *i = start_addr; i <= end_addr; ++i)
	{
		bool is_failed = false;
		for (size_t j = 0; j < mask_len; ++j)
		{
			uint8_t mask_byte		= mask[j];
			uint8_t scan_byte		= *(i + j);
			uint8_t pattern_byte	= pattern[j];

			if (mask_byte != '?' && pattern_byte != scan_byte)
			{
				is_failed = true;
				break;
			}
		}
		if (!is_failed)
			return i;
	}
	return nullptr;
}

bool GetModuleInfo(HANDLE proc_handle, HMODULE module_handle, module_info_t &module_info)
{
	MODULEINFO minfo;
	if (!GetModuleInformation(proc_handle, module_handle, &minfo, sizeof(minfo)))
		return false;

	module_info.base_addr			= (uint8_t*)minfo.lpBaseOfDll;
	module_info.image_size			= minfo.SizeOfImage;
	module_info.entry_point_addr	= (uint8_t*)minfo.EntryPoint;
	return true;
}

void *FindMemoryValue(uint32_t *start_addr, size_t scan_len, uint32_t value)
{
	uint32_t *end_addr = (start_addr + scan_len) - sizeof(value);
	for (uint32_t *i = start_addr; i <= end_addr; ++i)
	{
		if (*i == value)
			return i;
	}
	return nullptr;
}

void FindServerModule(HMODULE &module_handle)
{
	size_t names_count;
	const char *lib_names[] =
	{
		// TODO: parsing this names from external file
		"mp.dll",
		"hl.dll",
		"opfor.dll",
		"echoes.dll",
	};

	names_count = sizeof(lib_names) / sizeof(*lib_names);
	for (size_t i = 0; i < names_count; ++i)
	{
		module_handle = GetModuleHandle(lib_names[i]);
		if (module_handle)
			return;
	}
}

