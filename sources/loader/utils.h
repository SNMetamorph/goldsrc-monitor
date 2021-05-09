#pragma once
#include "moduleinfo.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <stdint.h>

namespace Utils
{
    int FindProcessID(const wchar_t *processName);
    HMODULE FindProcessModule(HANDLE procHandle, const wchar_t *moduleName);
    bool FindLibraryPath(const std::wstring &libName, std::wstring &libPath);
    int GetThreadExitCode(HANDLE threadHandle);
    size_t GetFunctionOffset(HMODULE moduleHandle, const char *funcName);
    bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo);
    void GetProcessWindowList(DWORD processID, std::vector<HWND> &windowList);
}

