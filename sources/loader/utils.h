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
