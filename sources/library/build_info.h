#pragma once
#include "build_info_entry.h"
#include "moduleinfo.h"
#include "memory_pattern.h"
#include <vector>

class CBuildInfo
{
public:
    void Initialize(const moduleinfo_t &engineModule);
    void *FindFunctionAddress(FunctionType funcType, void *startAddr, void *endAddr = nullptr) const;

private:
    bool LoadBuildInfoFile(std::vector<uint8_t> &fileContents);
    void ParseBuildInfo(std::vector<uint8_t> &fileContents);
    int GetBuildNumber() const;
    int DateToBuildNumber(const char *date) const;
    const char *FindDateString(uint8_t *startAddr, int maxLen) const;
    bool ApproxBuildNumber(const moduleinfo_t &engineModule);
    bool FindBuildNumberFunc(const moduleinfo_t &engineModule);

    int m_iBuildNumber;
    int (*m_pfnGetBuildNumber)() = nullptr;
    std::vector<CBuildInfoEntry> m_InfoEntries;
    std::vector<CMemoryPattern> m_BuildNumberSignatures;
};
