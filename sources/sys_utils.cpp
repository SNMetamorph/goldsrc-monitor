/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include "sys_utils.h"
#include <vector>
#include <cassert>
#include <algorithm>

#ifdef _WIN32
#include <Psapi.h>
#include <TlHelp32.h>
#endif

static ModuleHandle g_CurrentModuleHandle = 0;

bool ProcessHandle::Valid() const
{
    return m_iHandle > 0;
}

bool ModuleHandle::Valid() const
{
    return m_iHandle > 0;
}

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

#ifdef _WIN32
ProcessHandle::ProcessHandle(HANDLE handle)
{
    m_iHandle = reinterpret_cast<int64_t>(handle);
}

ProcessHandle::operator HANDLE() const
{
    return reinterpret_cast<HANDLE>(m_iHandle);
}

ModuleHandle::ModuleHandle(HMODULE handle)
{
    m_iHandle = reinterpret_cast<int64_t>(handle);
}

ModuleHandle::operator HMODULE() const
{
    return reinterpret_cast<HMODULE>(m_iHandle);
}

void SysUtils::Sleep(size_t timeMsec)
{
    ::Sleep(timeMsec);
}

ModuleHandle SysUtils::GetCurrentProcessModule()
{
    return GetModuleHandle(NULL);
}

ProcessHandle SysUtils::GetCurrentProcessHandle()
{
    return GetCurrentProcess();
}

bool SysUtils::GetModuleDirectory(ModuleHandle moduleHandle, std::string &workingDir)
{
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
}

bool SysUtils::GetModuleFilename(ModuleHandle moduleHandle, std::string &fileName)
{
    fileName.resize(MAX_PATH);
    if (GetModuleFileNameA(moduleHandle, fileName.data(), fileName.capacity())) 
    {
        fileName.assign(fileName.c_str());
        fileName.erase(0, fileName.find_last_of("/\\") + 1);
        fileName.shrink_to_fit();
        return true;
    }
    return false;
}

bool SysUtils::GetModuleInfo(ProcessHandle procHandle, ModuleHandle moduleHandle, SysUtils::ModuleInfo &moduleInfo)
{
    MODULEINFO minfo;
    if (GetModuleInformation(procHandle, moduleHandle, &minfo, sizeof(minfo))) 
    {
        moduleInfo.baseAddress = (uint8_t *)minfo.lpBaseOfDll;
        moduleInfo.imageSize = minfo.SizeOfImage;
        moduleInfo.entryPointAddress = (uint8_t *)minfo.EntryPoint;
        return true;
    }
    return false;
}

ModuleHandle SysUtils::FindModuleByExport(ProcessHandle procHandle, const char *exportName)
{
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
}

ModuleHandle SysUtils::FindModuleInProcess(ProcessHandle procHandle, const std::string &moduleName)
{
    size_t  modulesCount;
    HMODULE moduleHandle;
    std::string fileName;
    std::vector<HMODULE> modulesList;
    static char modulePath[MAX_PATH];

    DWORD listSize = 0;
    EnumProcessModules(procHandle, NULL, 0, &listSize);
    moduleHandle = NULL;
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
        const size_t pathLength = sizeof(modulePath) / sizeof(modulePath[0]);
        GetModuleFileNameExA(procHandle, modulesList[i], modulePath, pathLength);
        fileName.assign(modulePath);
        fileName.erase(0, fileName.find_last_of("/\\") + 1);

        bool stringsEqual = true;
        if (fileName.length() == moduleName.length())
        {
            for (size_t i = 0; i < fileName.length(); i++)
            {
                if (std::tolower(fileName[i]) != std::tolower(moduleName[i]))
                {
                    stringsEqual = false;
                    break;
                }
            }
        }
        else {
            stringsEqual = false;
        }

        if (stringsEqual)
        {
            moduleHandle = modulesList[i];
            break;
        }
    }
    return moduleHandle;
}

void SysUtils::FindProcessIdByName(const char *processName, std::vector<int32_t>& processIds)
{
#undef Process32First
#undef Process32Next
#undef PROCESSENTRY32
    HANDLE			processSnap;
    PROCESSENTRY32	processEntry;

    processIds.clear();
    processEntry.dwSize = sizeof(processEntry);
    processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (processSnap != INVALID_HANDLE_VALUE)
    {
        if (Process32First(processSnap, &processEntry))
        {
            do {
                if (std::strncmp(processName, processEntry.szExeFile, sizeof(processEntry.szExeFile)) == 0) {
                    processIds.push_back(processEntry.th32ProcessID);
                }
            } while (Process32Next(processSnap, &processEntry));
        }
        CloseHandle(processSnap);
    }
}

void *SysUtils::GetModuleFunction(ModuleHandle moduleHandle, const char *funcName)
{
    return GetProcAddress(moduleHandle, funcName);
}
#endif
