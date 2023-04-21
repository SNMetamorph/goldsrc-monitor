#include "utils.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <shlwapi.h>

namespace fs = std::filesystem;

int Utils::GetThreadExitCode(HANDLE threadHandle)
{
    DWORD exitCode;
    GetExitCodeThread(threadHandle, &exitCode);
    return exitCode;
}

bool Utils::FindLibraryAbsolutePath(const std::string &libraryName, std::filesystem::path &libPath)
{
    fs::path libraryRelativePath = libraryName;
    if (fs::exists(libraryRelativePath)) {
        libPath = fs::absolute(libraryRelativePath);
        return true;
    }
    return false;
}

size_t Utils::GetFunctionOffset(ModuleHandle moduleHandle, const char *funcName)
{
    SysUtils::ModuleInfo moduleInfo;
    SysUtils::GetModuleInfo(SysUtils::GetCurrentProcessHandle(), moduleHandle, moduleInfo);
    uint8_t *funcAddr = reinterpret_cast<uint8_t*>(SysUtils::GetModuleFunction(moduleHandle, funcName));
    return (size_t)(funcAddr - moduleInfo.baseAddress);
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
