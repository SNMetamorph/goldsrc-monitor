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
#include "build_info.h"
#include "memory_pattern.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <vector>

class CBuildInfo::Impl
{
public:
    typedef int(__cdecl *pfnGetBuildNumber_t)();

	int GetBuildNumber() const;
    int DateToBuildNumber(const char *date) const;
    const char *FindDateString(uint8_t *startAddr, int maxLen) const;

    bool LoadBuildInfoFile(std::vector<uint8_t> &fileContents);
    void ParseBuildInfo(std::vector<uint8_t> &fileContents);
    void ParseBuildInfoEntry(CBuildInfo::Entry &destEntry, const rapidjson::Value &jsonObject);
    bool ApproxBuildNumber(const SysUtils::ModuleInfo &engineModule);
    bool FindBuildNumberFunc(const SysUtils::ModuleInfo &engineModule);
    int FindActualInfoEntry();

    int m_iBuildNumber = -1;
    int m_iActualEntryIndex = -1;
    pfnGetBuildNumber_t m_pfnGetBuildNumber = nullptr;
    std::vector<CBuildInfo::Entry> m_InfoEntries;
    std::vector<CMemoryPattern> m_BuildNumberSignatures;
};
