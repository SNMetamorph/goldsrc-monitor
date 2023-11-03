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
#include <string>
#include <vector>
#include <optional>

class CBuildInfo::Impl
{
public:
    typedef int(__cdecl *pfnGetBuildNumber_t)();

	std::optional<uint32_t> GetBuildNumber() const;
    int DateToBuildNumber(const char *date) const;
    const char *FindDateString(uint8_t *startAddr, int maxLen) const;

    bool LoadBuildInfoFile(std::vector<uint8_t> &fileContents);
    std::optional<std::string> ParseBuildInfo(std::vector<uint8_t> &fileContents);
    std::optional<std::string> ParseBuildInfoEntry(CBuildInfo::Entry &destEntry, const rapidjson::Value &jsonObject);
    bool ApproxBuildNumber(const SysUtils::ModuleInfo &engineModule);
    bool FindBuildNumberFunc(const SysUtils::ModuleInfo &engineModule);
    std::optional<size_t> FindActualInfoEntry();

    uint32_t m_iBuildNumber = -1;
    bool m_infoEntryGameSpecific = false;
    pfnGetBuildNumber_t m_pfnGetBuildNumber = nullptr;
    std::string m_initErrorMessage;
    std::optional<size_t> m_iActualEntryIndex = std::nullopt;
    std::vector<CBuildInfo::Entry> m_GameInfoEntries;
    std::vector<CBuildInfo::Entry> m_EngineInfoEntries;
    std::vector<CMemoryPattern> m_BuildNumberSignatures;
};
