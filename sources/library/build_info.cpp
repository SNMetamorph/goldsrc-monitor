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
    if (m_pImpl->LoadBuildInfoFile(fileContents)) 
    {
        auto error = m_pImpl->ParseBuildInfo(fileContents);
        if (error.has_value()) {
            m_pImpl->m_initErrorMessage = error.value();
            return;
        }
    }
    else {
        m_pImpl->m_initErrorMessage = "failed to load build info file";
        return;
    }
    if (!m_pImpl->FindBuildNumberFunc(engineModule))
    {
        if (!m_pImpl->ApproxBuildNumber(engineModule)) {
            m_pImpl->m_initErrorMessage = "failed to approximate engine build number";
            return;
        }
    }

    auto entryIndex = m_pImpl->FindActualInfoEntry();
    if (!entryIndex.has_value()) {
        m_pImpl->m_initErrorMessage = "not found matching build info entry";
        return;
    }
    m_pImpl->m_iActualEntryIndex = entryIndex;
}

void *CBuildInfo::FindFunctionAddress(CBuildInfo::FunctionType funcType, void *startAddr, void *endAddr) const
{
    if (!m_pImpl->m_iActualEntryIndex.has_value()) {
        return nullptr;
    }

    const CBuildInfo::Entry *entry;
    if (m_pImpl->m_infoEntryGameSpecific) {
        entry = m_pImpl->m_GameInfoEntries.data() + m_pImpl->m_iActualEntryIndex.value();
    }
    else {
        entry = m_pImpl->m_EngineInfoEntries.data() + m_pImpl->m_iActualEntryIndex.value();
    }

    CMemoryPattern funcPattern = entry->GetFunctionPattern(funcType);
    if (!endAddr)
        endAddr = (uint8_t *)startAddr + funcPattern.GetLength();

    return Utils::FindPatternAddress(
        startAddr,
        endAddr,
        funcPattern
    );
}

const std::string &CBuildInfo::GetInitErrorDescription() const
{
    return m_pImpl->m_initErrorMessage;
}

const CBuildInfo::Entry *CBuildInfo::GetInfoEntry() const
{
    if (!m_pImpl->m_iActualEntryIndex.has_value()) {
        return nullptr;
    }
    if (m_pImpl->m_infoEntryGameSpecific) {
        return m_pImpl->m_GameInfoEntries.data() + m_pImpl->m_iActualEntryIndex.value();
    }
    else {
        return m_pImpl->m_EngineInfoEntries.data() + m_pImpl->m_iActualEntryIndex.value();
    }
}
