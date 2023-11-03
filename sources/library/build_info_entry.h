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
#include <string>
#include <stdint.h>

class CBuildInfo::Entry
{
public:
    Entry() {};
    bool Validate() const;
    inline uint32_t GetBuildNumber() const { return m_iBuildNumber; }
    inline void SetBuildNumber(uint32_t value) { m_iBuildNumber = value; }
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
    uint32_t m_iBuildNumber = 0;
    std::string m_szGameProcessName;
    uint64_t m_iClientEngfuncsOffset = 0x0;
    uint64_t m_iServerEngfuncsOffset = 0x0;
    CMemoryPattern m_FunctionPatterns[4];
};
