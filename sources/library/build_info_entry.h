#pragma once
#include "memory_pattern.h"
#include <string>

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
    inline int GetBuildNumber() const { return m_iBuildNumber; }
    inline void SetBuildNumber(int value) { m_iBuildNumber = value; }
    inline void SetFunctionPattern(FunctionType type, CMemoryPattern pattern) { 
        m_FunctionPatterns[type] = pattern; 
    }
    inline const CMemoryPattern &GetFunctionPattern(FunctionType type) const {
        return m_FunctionPatterns[type];
    }

private:
    int m_iBuildNumber;
    CMemoryPattern m_FunctionPatterns[FUNCTYPE_COUNT];
};
