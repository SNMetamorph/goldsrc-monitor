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

#include "utils.h"

namespace fs = std::filesystem;

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
