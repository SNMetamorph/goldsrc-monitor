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

#include "server_module.h"
#include "engine_module.h"
#include "build_info.h"
#include "build_info_entry.h"
#include "exception.h"
#include "sys_utils.h"
#include "utils.h"

enginefuncs_t *g_pServerEngfuncs;
CServerModule &g_ServerModule = CServerModule::GetInstance();

CServerModule &CServerModule::GetInstance()
{
    static CServerModule instance;
    return instance;
}

bool CServerModule::FindHandle()
{
    ProcessHandle currProcess = SysUtils::GetCurrentProcessHandle();
    m_hModule = SysUtils::FindModuleByExport(GetCurrentProcess(), "GetEntityAPI");
    SysUtils::GetModuleInfo(currProcess, m_hModule, m_ModuleInfo);
    return m_hModule != NULL;
}

bool CServerModule::FindEngfuncs(const CBuildInfo &buildInfo)
{
    void *pfnPrecacheModel;
    void *pfnPrecacheSound;
    uint8_t *probeAddr;
    uint8_t *coincidenceAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleEndAddr;
    uint8_t *moduleAddr;

    const size_t pointerSize = sizeof(void *);
    moduleAddr = g_EngineModule.GetAddress();
    moduleEndAddr = moduleAddr + g_EngineModule.GetSize();

    const CBuildInfo::Entry &buildInfoEntry = buildInfo.GetInfoEntry();
    if (buildInfoEntry.HasServerEngfuncsOffset()) {
        g_pServerEngfuncs = (enginefuncs_t *)(moduleAddr + buildInfoEntry.GetServerEngfuncsOffset());
        return true;
    }

    pfnPrecacheModel = buildInfo.FindFunctionAddress(
        CBuildInfo::FunctionType::PrecacheModel,
        moduleAddr,
        moduleEndAddr
    );

    if (!pfnPrecacheModel) 
    {
        // PrecacheModel() address not found
        return false;
    }

    scanStartAddr = moduleAddr;
    while (true)
    {
        coincidenceAddr = (uint8_t *)Utils::FindMemoryPointer(
            scanStartAddr,
            moduleEndAddr,
            pfnPrecacheModel
        );
        if (!coincidenceAddr || scanStartAddr >= moduleEndAddr) 
        {
            // valid pointer to PrecacheModel() not found
            return false;
        }
        else
            scanStartAddr = coincidenceAddr + pointerSize;

        probeAddr = *(uint8_t **)(coincidenceAddr + pointerSize);
        if (probeAddr >= moduleAddr && probeAddr < moduleEndAddr)
        {
            pfnPrecacheSound = buildInfo.FindFunctionAddress(
                CBuildInfo::FunctionType::PrecacheSound, probeAddr
            );
            if (pfnPrecacheSound)
            {
                g_pServerEngfuncs = (enginefuncs_t *)coincidenceAddr;
                return true;
            }
        }
    }
    return false;
}

uint8_t* CServerModule::GetFuncAddress(const char *funcName)
{
    return reinterpret_cast<uint8_t*>(GetProcAddress(GetHandle(), funcName));
}
