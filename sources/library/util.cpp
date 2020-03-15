#include "util.h"
#include "moduleinfo.h"

#include <stdint.h>
#include <cstring>
#include <vector>
#include <Windows.h>
#include <Psapi.h>

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
            uint8_t maskByte = mask[j];
            uint8_t scanByte = *(i + j);
            uint8_t patternByte = pattern[j];

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

HMODULE FindModuleByExport(HANDLE procHandle, const char *exportName)
{
    DWORD listSize;
    size_t modulesCount;
    std::vector<HMODULE> modulesList;

    // retrieve modules count
    listSize = 0;
    EnumProcessModules(procHandle, NULL, 0, &listSize);
    modulesCount = listSize / sizeof(HMODULE);

    if (modulesCount > 0)
        modulesList.resize(modulesCount);
    else
        return NULL;

    if (!EnumProcessModules(procHandle, modulesList.data(), listSize, &listSize))
        return NULL;

    for (size_t i = 0; i < modulesCount; ++i)
    {
        uint8_t *moduleAddr;
        uint32_t *nameOffsetList;
        PIMAGE_DOS_HEADER dosHeader;
        PIMAGE_NT_HEADERS peHeader;
        PIMAGE_EXPORT_DIRECTORY dllExports;

        moduleAddr  = (uint8_t*)modulesList[i];
        dosHeader   = (PIMAGE_DOS_HEADER)moduleAddr;
        peHeader    = (PIMAGE_NT_HEADERS)(moduleAddr + dosHeader->e_lfanew);
        dllExports  = (PIMAGE_EXPORT_DIRECTORY)(moduleAddr +
            peHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        if (!dllExports->AddressOfNames)
            continue;

        nameOffsetList = (uint32_t*)(moduleAddr + dllExports->AddressOfNames);
        for (size_t j = 0; j < dllExports->NumberOfNames; ++j)
        {
            const char *entryName = (const char *)(moduleAddr + nameOffsetList[j]);
            if (strcmp(entryName, exportName) == 0)
                return modulesList[i];
        }
    }
    return NULL;
}

bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo)
{
    MODULEINFO minfo;
    if (!GetModuleInformation(procHandle, moduleHandle, &minfo, sizeof(minfo)))
        return false;

    moduleInfo.baseAddr = (uint8_t*)minfo.lpBaseOfDll;
    moduleInfo.imageSize = minfo.SizeOfImage;
    moduleInfo.entryPointAddr = (uint8_t*)minfo.EntryPoint;
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
