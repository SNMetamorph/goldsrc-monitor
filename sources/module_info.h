#pragma once
#include <stdint.h>
#include <Windows.h>

struct ModuleInfo
{
    size_t imageSize;
    uint8_t *baseAddress;
    uint8_t *entryPointAddress;
};

namespace Utils 
{
    bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, ModuleInfo &moduleInfo);
};
