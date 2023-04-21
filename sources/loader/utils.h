#pragma once
#include "sys_utils.h"
#include <string>
#include <stdint.h>
#include <filesystem>

namespace Utils
{
    bool FindLibraryAbsolutePath(const std::string &libName, std::filesystem::path &libPath);
    size_t GetFunctionOffset(ModuleHandle moduleHandle, const char *funcName);
}
