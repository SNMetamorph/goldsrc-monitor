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
CClientModule& g_ClientModule = CClientModule::GetInstance();

CClientModule& CClientModule::GetInstance()
{
    static CClientModule instance;
    return instance;
}

bool CClientModule::FindHandle()
{
    ProcessHandle currProcess = SysUtils::GetCurrentProcessHandle();
    m_hModule = SysUtils::FindModuleByExport(currProcess, "HUD_ProcessPlayerState");
    SysUtils::GetModuleInfo(currProcess, m_hModule, m_ModuleInfo);
    return m_hModule.Valid();
}

bool CClientModule::FindEngfuncs(const CBuildInfo &buildInfo)
{
    uint8_t *probeAddr;
    uint8_t *coincidenceAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleEndAddr;
    uint8_t *moduleAddr;
    uint8_t *pfnSPR_Load;
    uint8_t *pfnSPR_Frames;
    const CBuildInfo::Entry &buildInfoEntry = buildInfo.GetInfoEntry();
    const size_t pointerSize = sizeof(void *);

    moduleAddr = g_EngineModule.GetAddress();
    scanStartAddr = moduleAddr;
    moduleEndAddr = moduleAddr + g_EngineModule.GetSize();
    
    // obtain address directly without searching
    if (buildInfoEntry.HasClientEngfuncsOffset()) {
        g_pClientEngfuncs = (cl_enginefunc_t *)(moduleAddr + buildInfoEntry.GetClientEngfuncsOffset());
        return true;
    }

    if (!g_EngineModule.GetFunctionsFromAPI(&pfnSPR_Load, &pfnSPR_Frames))
    {
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
    
    bool fallbackMethod = false;
    uint8_t *targetAddr = pfnSPR_Load;
    while (true)
    {
        coincidenceAddr = (uint8_t *)Utils::FindMemoryPointer(
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
        probeAddr = *(uint8_t **)(coincidenceAddr + pointerSize);
        if (probeAddr >= moduleAddr && probeAddr < moduleEndAddr)
        {
            if (probeAddr == pfnSPR_Frames || (fallbackMethod && Utils::UnwrapJmp(probeAddr) == pfnSPR_Frames))
            {
                g_pClientEngfuncs = (cl_enginefunc_t *)coincidenceAddr;
                return true; 
            }
        }
    }

    EXCEPT("valid reference to SPR_Load() not found");
    return false;
}

uint8_t* CClientModule::GetFuncAddress(const char *funcName)
{
    return reinterpret_cast<uint8_t*>(GetProcAddress(GetHandle(), funcName));
}

CClientModule::CClientModule()
{
    m_ModuleInfo.baseAddress = nullptr;
    m_ModuleInfo.entryPointAddress = nullptr;
    m_ModuleInfo.imageSize = 0;
}
