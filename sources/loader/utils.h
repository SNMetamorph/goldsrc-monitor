#pragma once
#include "sys_utils.h"
#include <vector>
#include <string>
#include <stdint.h>
#include <filesystem>
#include <Windows.h>

namespace Utils
{
    bool FindLibraryAbsolutePath(const std::string &libName, std::filesystem::path &libPath);
    int GetThreadExitCode(HANDLE threadHandle);
    size_t GetFunctionOffset(ModuleHandle moduleHandle, const char *funcName);
    void GetProcessWindowList(DWORD processID, std::vector<HWND> &windowList);
}
