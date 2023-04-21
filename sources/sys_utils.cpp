#include "sys_utils.h"
#include <vector>
#include <cassert>

#ifdef _WIN32
#include <Psapi.h>
#endif

static ModuleHandle g_CurrentModuleHandle = 0;

void SysUtils::InitCurrentLibraryHandle(ModuleHandle handle)
{
    static bool initialized = false;
    if (!initialized) {
        g_CurrentModuleHandle = handle;
        initialized = true;
    }
}

ModuleHandle SysUtils::GetCurrentLibraryHandle()
{
    assert(g_CurrentModuleHandle != 0);
    return g_CurrentModuleHandle;
}

ModuleHandle SysUtils::GetCurrentProcessModule()
{
#ifdef _WIN32
    return GetModuleHandle(NULL);
#else
    return -1;
#endif
}

ProcessHandle SysUtils::GetCurrentProcessHandle()
{
#ifdef _WIN32
    return GetCurrentProcess();
#else
    return 0;
#endif
}

bool SysUtils::GetModuleDirectory(ModuleHandle moduleHandle, std::string &workingDir)
{
#ifdef _WIN32
    workingDir.resize(MAX_PATH);
    GetModuleFileNameA(
        moduleHandle,
        workingDir.data(),
        workingDir.capacity()
    );

    if (std::strlen(workingDir.c_str()) > 1)
    {
        // remove file name
        workingDir.assign(workingDir.c_str());
        workingDir.erase(workingDir.find_last_of("/\\") + 1);
        workingDir.shrink_to_fit();
        return true;
    }
    else {
        return false;
    }
#else
    return false;
#endif
}

bool SysUtils::GetModuleFilename(ModuleHandle moduleHandle, std::string &fileName)
{
#ifdef _WIN32
    fileName.resize(MAX_PATH);
    if (GetModuleFileNameA(moduleHandle, fileName.data(), fileName.capacity())) 
    {
        fileName.assign(fileName.c_str());
        fileName.erase(0, fileName.find_last_of("/\\") + 1);
        fileName.shrink_to_fit();
        return true;
    }
    return false;
#else
    return false;
#endif
}

bool SysUtils::GetModuleInfo(ProcessHandle procHandle, ModuleHandle moduleHandle, SysUtils::ModuleInfo &moduleInfo)
{
#ifdef _WIN32
    MODULEINFO minfo;
    if (GetModuleInformation(procHandle, moduleHandle, &minfo, sizeof(minfo))) 
    {
        moduleInfo.baseAddress = (uint8_t *)minfo.lpBaseOfDll;
        moduleInfo.imageSize = minfo.SizeOfImage;
        moduleInfo.entryPointAddress = (uint8_t *)minfo.EntryPoint;
        return true;
    }
    return false;
#else
    return false;
#endif
}

ModuleHandle SysUtils::FindModuleByExport(ProcessHandle procHandle, const char *exportName)
{
#ifdef _WIN32
    DWORD listSize;
    size_t modulesCount;
    std::vector<HMODULE> modulesList;

    // retrieve modules count
    listSize = 0;
    EnumProcessModules(procHandle, NULL, 0, &listSize);
    modulesCount = listSize / sizeof(HMODULE);

    if (modulesCount > 0) {
        modulesList.resize(modulesCount);
    }
    else {
        return NULL;
    }

    if (!EnumProcessModules(procHandle, modulesList.data(), listSize, &listSize)) {
        return NULL;
    }

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

        if (!dllExports->AddressOfNames) {
            continue;
        }

        nameOffsetList = (uint32_t*)(moduleAddr + dllExports->AddressOfNames);
        for (size_t j = 0; j < dllExports->NumberOfNames; ++j)
        {
            const char *entryName = (const char *)(moduleAddr + nameOffsetList[j]);
            if (strcmp(entryName, exportName) == 0)
                return modulesList[i];
        }
    }
    return NULL;
#else
    return 0;
#endif
}
