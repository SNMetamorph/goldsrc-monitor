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

#include "build_info.h"
#include "build_info_entry.h"
#include "build_info_impl.h"
#include "exception.h"
#include "utils.h"
#include <vector>

CBuildInfo::CBuildInfo()
{
    m_pImpl = std::make_unique<CBuildInfo::Impl>();
}

CBuildInfo::~CBuildInfo()
{
}

void CBuildInfo::Initialize(const SysUtils::ModuleInfo &engineModule)
{
    std::vector<uint8_t> fileContents;
    if (m_pImpl->LoadBuildInfoFile(fileContents)) {
        m_pImpl->ParseBuildInfo(fileContents);
    }
    else {
        EXCEPT("failed to load build info file");
    }
    if (!m_pImpl->FindBuildNumberFunc(engineModule))
    {
        if (!m_pImpl->ApproxBuildNumber(engineModule)) {
            EXCEPT("failed to approximate engine build number");
        }
    }
    m_pImpl->m_iActualEntryIndex = m_pImpl->FindActualInfoEntry();
    if (m_pImpl->m_iActualEntryIndex < 0) {
        EXCEPT("no one build info entries parsed");
    }
}

void *CBuildInfo::FindFunctionAddress(CBuildInfo::FunctionType funcType, void *startAddr, void *endAddr) const
{
    CMemoryPattern funcPattern = m_pImpl->m_InfoEntries[m_pImpl->m_iActualEntryIndex].GetFunctionPattern(funcType);
    if (!endAddr)
        endAddr = (uint8_t *)startAddr + funcPattern.GetLength();

    return Utils::FindPatternAddress(
        startAddr,
        endAddr,
        funcPattern
    );
}

const CBuildInfo::Entry &CBuildInfo::GetInfoEntry() const
{
    return m_pImpl->m_InfoEntries[m_pImpl->m_iActualEntryIndex];
}
