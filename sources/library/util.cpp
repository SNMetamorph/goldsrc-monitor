#include "util.h"
#include "moduleinfo.h"

#include <cstring>
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>


void *FindPatternAddress(
    void *startAddr, void *endAddr, const char *pattern, const char *mask)
{
    bool isFailed;
    size_t maskLen;
    uint8_t *totalEndAddr;

    maskLen = strlen(mask);
    totalEndAddr = (uint8_t*)endAddr - maskLen;
	for (uint8_t *i = (uint8_t*)startAddr; i <= totalEndAddr; ++i)
	{
		isFailed = false;
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

void *FindMemoryInt32(void *startAddr, void *endAddr, uint32_t scanValue)
{
    void *valueAddr;
    HANDLE procHandle;
    uint32_t probeValue;
    uint32_t *totalEndAddr;

    valueAddr       = nullptr;
    procHandle      = GetCurrentProcess();
    totalEndAddr    = (uint32_t*)((size_t)endAddr - sizeof(scanValue));

	for (uint32_t *i = (uint32_t*)startAddr; i <= totalEndAddr; ++i)
	{
        if (!ReadProcessMemory(procHandle, i, &probeValue, sizeof(*i), NULL))
            continue;
      
        if (probeValue == scanValue)
        {
            valueAddr = i;
            break;
        }
	}
	return valueAddr;
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

