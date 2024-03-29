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

#include "client_module.h"
#include "engine_module.h"
#include "build_info.h"
#include "build_info_entry.h"
#include "exception.h"
#include "utils.h"
#include "sys_utils.h"

cl_enginefunc_t *g_pClientEngfuncs;

CClientModule::CClientModule(const CEngineModule &moduleRef)
    : m_engineModule(moduleRef)
{
    m_moduleInfo.baseAddress = nullptr;
    m_moduleInfo.entryPointAddress = nullptr;
    m_moduleInfo.imageSize = 0;
}

bool CClientModule::FindHandle()
{
    ProcessHandle currProcess = SysUtils::GetCurrentProcessHandle();
    m_moduleHandle = SysUtils::FindModuleByExport(currProcess, "HUD_ProcessPlayerState");
    SysUtils::GetModuleInfo(currProcess, m_moduleHandle, m_moduleInfo);
    return m_moduleHandle.Valid();
}

bool CClientModule::FindEngfuncs(const CBuildInfo &buildInfo)
{
    uint8_t *pfnSPR_Load;
    uint8_t *pfnSPR_Frames;
    uint8_t *moduleAddr = m_engineModule.GetAddress();
    uint8_t *moduleEndAddr = moduleAddr + m_engineModule.GetSize();
    const CBuildInfo::Entry *buildInfoEntry = buildInfo.GetInfoEntry();

    if (!m_engineModule.GetFunctionsFromAPI(&pfnSPR_Load, &pfnSPR_Frames))
    {
        if (!buildInfoEntry) {
            std::string errorMsg;
            Utils::Snprintf(errorMsg, "build info parsing error: %s\n", buildInfo.GetInitErrorDescription().c_str());
            EXCEPT(errorMsg);
        }
        // obtain address directly without searching
        if (buildInfoEntry->HasClientEngfuncsOffset()) {
            g_pClientEngfuncs = (cl_enginefunc_t *)(moduleAddr + buildInfoEntry->GetClientEngfuncsOffset());
            return true;
        }

        pfnSPR_Load = static_cast<uint8_t *>(buildInfo.FindFunctionAddress(
            CBuildInfo::FunctionType::SPR_Load, moduleAddr, moduleEndAddr
            ));
        if (!pfnSPR_Load) {
            EXCEPT("SPR_Load() address not found");
        }

        pfnSPR_Frames = static_cast<uint8_t *>(buildInfo.FindFunctionAddress(
            CBuildInfo::FunctionType::SPR_Frames, moduleAddr, moduleEndAddr
            ));
        if (!pfnSPR_Frames) {
            EXCEPT("SPR_Frames() address not found");
        }
    }

    cl_enginefunc_t *tableAddr = SearchEngfuncsTable(pfnSPR_Load, pfnSPR_Frames);
    if (tableAddr) {
        g_pClientEngfuncs = tableAddr;
        return true;
    }

    EXCEPT("valid reference to SPR_Load() not found");
    return false;
}

cl_enginefunc_t *CClientModule::SearchEngfuncsTable(uint8_t *pfnSPR_Load, uint8_t *pfnSPR_Frames)
{
    bool fallbackMethod = false;
    uint8_t *targetAddr = pfnSPR_Load;
    uint8_t *moduleAddr = m_engineModule.GetAddress();
    uint8_t *scanStartAddr = moduleAddr;
    uint8_t *moduleEndAddr = moduleAddr + m_engineModule.GetSize();
    constexpr size_t pointerSize = sizeof(void *);

    while (true)
    {
        uint8_t *coincidenceAddr = (uint8_t *)Utils::FindMemoryPointer(
            scanStartAddr,
            moduleEndAddr,
            targetAddr
        );
        if (!coincidenceAddr || scanStartAddr >= moduleEndAddr)
        {
            // try to use fallback method
            targetAddr = (uint8_t *)Utils::FindJmpFromAddress(moduleAddr, moduleEndAddr, pfnSPR_Load);
            if (!targetAddr || fallbackMethod) {
                break;
            }
            else
            {
                fallbackMethod = true;
                scanStartAddr = moduleAddr;
                continue;
            }
        }
        else {
            scanStartAddr = coincidenceAddr + pointerSize;
        }

        // check for module range to avoid segfault
        uint8_t *probeAddr = *(uint8_t **)(coincidenceAddr + pointerSize);
        if (probeAddr >= moduleAddr && probeAddr < moduleEndAddr)
        {
            if (probeAddr == pfnSPR_Frames || (fallbackMethod && Utils::UnwrapJmp(probeAddr) == pfnSPR_Frames))
            {
                return reinterpret_cast<cl_enginefunc_t*>(coincidenceAddr);
            }
        }
    }
    return nullptr;
}

uint8_t* CClientModule::GetFuncAddress(const char *funcName) const
{
    return reinterpret_cast<uint8_t*>(GetProcAddress(GetHandle(), funcName));
}
