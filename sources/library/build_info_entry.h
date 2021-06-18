#pragma once
#include "memory_pattern.h"
#include <string>
#include <stdint.h>

enum FunctionType
{
    FUNCTYPE_SPR_LOAD,
    FUNCTYPE_SPR_FRAMES,
    FUNCTYPE_PRECACHE_MODEL,
    FUNCTYPE_PRECACHE_SOUND,
    FUNCTYPE_COUNT, // keep this last
};

class CBuildInfoEntry
{
public:
    CBuildInfoEntry() {};
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
    inline void SetFunctionPattern(FunctionType type, CMemoryPattern pattern) { 
        m_FunctionPatterns[type] = pattern; 
    }
    inline const CMemoryPattern &GetFunctionPattern(FunctionType type) const {
        return m_FunctionPatterns[type];
    }
    inline bool operator<(const CBuildInfoEntry &operand) const {
        return m_iBuildNumber < operand.GetBuildNumber();
    }

private:
    int m_iBuildNumber = 0;
    std::string m_szGameProcessName;
    uint64_t m_iClientEngfuncsOffset = 0x0;
    uint64_t m_iServerEngfuncsOffset = 0x0;
    CMemoryPattern m_FunctionPatterns[FUNCTYPE_COUNT];
};
