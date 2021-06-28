#pragma once
#include "build_info_entry.h"
#include "module_info.h"
#include "memory_pattern.h"
#include <vector>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

class CBuildInfo
{
public:
    void Initialize(const ModuleInfo &engineModule);
    void *FindFunctionAddress(FunctionType funcType, void *startAddr, void *endAddr = nullptr) const;
    inline const CBuildInfoEntry &GetInfoEntry() const {
        return m_InfoEntries[m_iActualEntryIndex];
    };

private:
    int GetBuildNumber() const;
    int DateToBuildNumber(const char *date) const;
    const char *FindDateString(uint8_t *startAddr, int maxLen) const;

    bool LoadBuildInfoFile(std::vector<uint8_t> &fileContents);
    void ParseBuildInfo(std::vector<uint8_t> &fileContents);
    void ParseBuildInfoEntry(CBuildInfoEntry &destEntry, const rapidjson::Value &jsonObject);
    bool ApproxBuildNumber(const ModuleInfo &engineModule);
    bool FindBuildNumberFunc(const ModuleInfo &engineModule);
    int FindActualInfoEntry();

    int m_iBuildNumber = -1;
    int m_iActualEntryIndex = -1;
    int (*m_pfnGetBuildNumber)() = nullptr;
    std::vector<CBuildInfoEntry> m_InfoEntries;
    std::vector<CMemoryPattern> m_BuildNumberSignatures;
};
