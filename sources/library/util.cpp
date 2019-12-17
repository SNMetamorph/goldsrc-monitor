#include "util.h"
#include "moduleinfo.h"

#include <cstring>
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>


void *FindPatternAddress(
    void *startAddr, int scanLen, const char *pattern, const char *mask)
{
	size_t maskLen = strlen(mask);
    if (scanLen < 0)
        scanLen = maskLen;

	uint8_t *endAddr = ((uint8_t*)startAddr + scanLen) - maskLen;
	for (uint8_t *i = (uint8_t*)startAddr; i <= endAddr; ++i)
	{
		bool isFailed = false;
		for (size_t j = 0; j < maskLen; ++j)
		{
			uint8_t maskByte    = mask[j];
			uint8_t scanByte    = *(i + j);
			uint8_t patternByte	= pattern[j];

			if (maskByte != '?' && patternByte != scanByte)
			{
				isFailed = true;
				break;
			}
		}
		if (!isFailed)
			return i;
	}
	return nullptr;
}

bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo)
{
	MODULEINFO minfo;
	if (!GetModuleInformation(procHandle, moduleHandle, &minfo, sizeof(minfo)))
		return false;

	moduleInfo.baseAddr         = (uint8_t*)minfo.lpBaseOfDll;
	moduleInfo.imageSize        = minfo.SizeOfImage;
	moduleInfo.entryPointAddr   = (uint8_t*)minfo.EntryPoint;
	return true;
}

void *FindMemoryValue(void *startAddr, size_t scanLen, uint32_t value)
{
	uint32_t *end_addr = ((uint32_t*)startAddr + scanLen) - sizeof(value);
	for (uint32_t *i = (uint32_t*)startAddr; i <= end_addr; ++i)
	{
		if (*i == value)
			return i;
	}
	return nullptr;
}

void FindServerModule(HMODULE &moduleHandle)
{
	size_t namesCount;
	static const char *libNames[] =
	{
		// TODO: parsing this names from external file
		"mp.dll",
		"hl.dll",
		"opfor.dll",
		"echoes.dll",
	};

	namesCount = sizeof(libNames) / sizeof(*libNames);
	for (size_t i = 0; i < namesCount; ++i)
	{
		moduleHandle = GetModuleHandle(libNames[i]);
		if (moduleHandle)
			return;
	}
}

