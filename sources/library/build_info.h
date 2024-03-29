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
#include "sys_utils.h"
#include <memory>
#include <string>

class CBuildInfo
{
public:
    class Entry;
    enum class FunctionType
    {
        // client-side
        SPR_Load,
        SPR_Frames,
        // server-side
        PrecacheModel,
        PrecacheSound,
        Count, // keep this last
    };
    static const size_t k_functionsCount = static_cast<size_t>(FunctionType::Count);

    CBuildInfo();
    ~CBuildInfo();
    void Initialize(const SysUtils::ModuleInfo &engineModule);
    void *FindFunctionAddress(FunctionType funcType, void *startAddr, void *endAddr = nullptr) const;
    const std::string &GetInitErrorDescription() const;
    const Entry *GetInfoEntry() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};
