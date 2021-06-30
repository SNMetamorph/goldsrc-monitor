#include "utils.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <shlwapi.h>

bool Utils::FindProcessByName(const wchar_t *processName, std::vector<int> &processIds)
{
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
                if (wcscmp(processName, processEntry.szExeFile) == 0) {
                    processIds.push_back(processEntry.th32ProcessID);
                }
            } while (Process32Next(processSnap, &processEntry));
        }
        CloseHandle(processSnap);
    }
    return processIds.size() > 0;
}

HMODULE Utils::FindProcessModule(HANDLE procHandle, const wchar_t *moduleName)
{
    DWORD	listSize;
    size_t  modulesCount;
    HMODULE moduleHandle;
    std::vector<HMODULE> modulesList;
    static wchar_t modulePath[MAX_PATH];
    const size_t pathLength =
        sizeof(modulePath) / sizeof(modulePath[0]);

    listSize = 0;
    EnumProcessModules(procHandle, NULL, 0, &listSize);
    moduleHandle = NULL;
    modulesCount = listSize / sizeof(HMODULE);

    if (modulesCount > 0)
        modulesList.resize(modulesCount);
    else
        return NULL;

    if (!EnumProcessModules(procHandle, modulesList.data(), listSize, &listSize))
        return NULL;

    for (size_t i = 0; i < modulesCount; ++i)
    {
        GetModuleFileNameExW(procHandle, modulesList[i], modulePath, pathLength);
        wchar_t *fileName = wcsrchr(modulePath, '\\') + 1;

        // case insensetive comparing routine
        // standard wcscmp() checking fails on Win10
        if (_wcsicmp(fileName, moduleName) == 0)
        {
            moduleHandle = modulesList[i];
            break;
        }
    }
    return moduleHandle;
}

int Utils::GetThreadExitCode(HANDLE threadHandle)
{
    DWORD exitCode;
    GetExitCodeThread(threadHandle, &exitCode);
    return exitCode;
}

bool Utils::FindLibraryPath(const std::wstring &libraryName, std::wstring &libPath)
{
    libPath.reserve(MAX_PATH);
    GetFullPathNameW(
        libraryName.c_str(),
        libPath.capacity(),
        libPath.data(),
        NULL
    );

    if (PathFileExistsW(libPath.data()))
        return true;
    else
        return false;
}

size_t Utils::GetFunctionOffset(HMODULE moduleHandle, const char *funcName)
{
    uint8_t *funcAddr = (uint8_t *)GetProcAddress(moduleHandle, funcName);
    return (size_t)(funcAddr - (uint8_t*)moduleHandle);
}

void Utils::GetProcessWindowList(DWORD processID, std::vector<HWND> &windowList)
{
    HWND window = NULL;
    windowList.clear();
    do {
        DWORD checkProcessID;
        window = FindWindowEx(NULL, window, NULL, NULL);
        GetWindowThreadProcessId(window, &checkProcessID);
        if (processID == checkProcessID) {
            windowList.push_back(window);
        }
    } 
    while (window != NULL);
}
