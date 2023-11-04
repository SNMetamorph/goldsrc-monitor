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

    uint32_t GetBuildNumber() const { return m_buildNumber; }
    void SetBuildNumber(uint32_t value) { m_buildNumber = value; }

    bool HasClientEngfuncsOffset() const { return m_clientEngfuncsOffset != 0; }
    bool HasServerEngfuncsOffset() const { return m_serverEngfuncsOffset != 0; }
    uint64_t GetClientEngfuncsOffset() const { return m_clientEngfuncsOffset; }
    uint64_t GetServerEngfuncsOffset() const { return m_serverEngfuncsOffset; }
    void SetClientEngfuncsOffset(uint64_t offset) { m_clientEngfuncsOffset = offset; }
    void SetServerEngfuncsOffset(uint64_t offset) { m_serverEngfuncsOffset = offset; }

    const std::string &GetGameProcessName() const { return m_gameProcessName; }
    void SetGameProcessName(const char *value) { m_gameProcessName.assign(value); }

    void SetFunctionPattern(CBuildInfo::FunctionType type, CMemoryPattern pattern) { 
        m_functionPatterns[static_cast<size_t>(type)] = pattern; 
    }
    const CMemoryPattern &GetFunctionPattern(CBuildInfo::FunctionType type) const {
        return m_functionPatterns[static_cast<size_t>(type)];
    }

    bool operator<(const Entry &operand) const {
        return m_buildNumber < operand.GetBuildNumber();
    }

private:
    uint32_t m_buildNumber = 0;
    std::string m_gameProcessName;
    uint64_t m_clientEngfuncsOffset = 0x0;
    uint64_t m_serverEngfuncsOffset = 0x0;
    CMemoryPattern m_functionPatterns[CBuildInfo::k_functionsCount];
};
