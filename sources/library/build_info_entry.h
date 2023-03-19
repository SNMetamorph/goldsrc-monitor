#pragma once
#include "build_info.h"
#include "memory_pattern.h"
#include <string>
#include <stdint.h>

class CBuildInfo::Entry
{
public:
    Entry() {};
    bool Validate() const;
    inline int GetBuildNumber() const { return m_iBuildNumber; }
    inline void SetBuildNumber(int value) { m_iBuildNumber = value; }
    inline bool HasClientEngfuncsOffset() const { return m_iClientEngfuncsOffset != 0; }
    inline bool HasServerEngfuncsOffset() const { return m_iServerEngfuncsOffset != 0; }
    inline uint64_t GetClientEngfuncsOffset() const { return m_iClientEngfuncsOffset; }
    inline uint64_t GetServerEngfuncsOffset() const { return m_iServerEngfuncsOffset; }
    inline void SetClientEngfuncsOffset(uint64_t offset) { m_iClientEngfuncsOffset = offset; }
    inline void SetServerEngfuncsOffset(uint64_t offset) { m_iServerEngfuncsOffset = offset; }
    inline const std::string &GetGameProcessName() const { return m_szGameProcessName; }
    inline void SetGameProcessName(const char *value) { m_szGameProcessName.assign(value); }
    inline void SetFunctionPattern(CBuildInfo::FunctionType type, CMemoryPattern pattern) { 
        m_FunctionPatterns[static_cast<size_t>(type)] = pattern; 
    }
    inline const CMemoryPattern &GetFunctionPattern(CBuildInfo::FunctionType type) const {
        return m_FunctionPatterns[static_cast<size_t>(type)];
    }
    inline bool operator<(const Entry &operand) const {
        return m_iBuildNumber < operand.GetBuildNumber();
    }

private:
    int m_iBuildNumber = 0;
    std::string m_szGameProcessName;
    uint64_t m_iClientEngfuncsOffset = 0x0;
    uint64_t m_iServerEngfuncsOffset = 0x0;
    CMemoryPattern m_FunctionPatterns[4];
};
