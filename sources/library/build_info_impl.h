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
