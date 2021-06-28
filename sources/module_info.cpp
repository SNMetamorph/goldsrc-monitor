#include "module_info.h"
#include <Psapi.h>

bool Utils::GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, ModuleInfo &moduleInfo)
{
    MODULEINFO minfo;
    if (!GetModuleInformation(procHandle, moduleHandle, &minfo, sizeof(minfo)))
        return false;

    moduleInfo.baseAddress = (uint8_t *)minfo.lpBaseOfDll;
    moduleInfo.imageSize = minfo.SizeOfImage;
    moduleInfo.entryPointAddress = (uint8_t *)minfo.EntryPoint;
    return true;
}
