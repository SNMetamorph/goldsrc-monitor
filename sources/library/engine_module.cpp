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

#include "engine_module.h"
#include "hlsdk.h"

CEngineModule& g_EngineModule = CEngineModule::GetInstance();

CEngineModule& CEngineModule::GetInstance()
{
    static CEngineModule instance;
    return instance;
}

bool CEngineModule::FindHandle()
{
    m_moduleHandle = GetModuleHandle("hw.dll");
    if (!m_moduleHandle)
    {
        m_moduleHandle = GetModuleHandle("sw.dll");
        if (m_moduleHandle) 
        {
            m_isSoftwareRenderer = true;
        }
        else 
        {
            m_isXashEngine = true;
            m_moduleHandle = GetModuleHandle("xash.dll");
        }
    }
    return (m_moduleHandle != NULL) && SetupModuleInfo();
}

bool CEngineModule::GetFunctionsFromAPI(uint8_t **pfnSPR_Load, uint8_t **pfnSPR_Frames)
{
    if (m_isXashEngine)
    {
        pfnEngSrc_pfnSPR_Load_t pfnFunc1 = (pfnEngSrc_pfnSPR_Load_t)GetProcAddress(m_moduleHandle, "pfnSPR_Load");
        if (pfnFunc1)
        {
            pfnEngSrc_pfnSPR_Frames_t pfnFunc2 = (pfnEngSrc_pfnSPR_Frames_t)GetProcAddress(m_moduleHandle, "pfnSPR_Frames");
            if (pfnFunc2)
            {
                *pfnSPR_Load = reinterpret_cast<uint8*>(pfnFunc1);
                *pfnSPR_Frames = reinterpret_cast<uint8 *>(pfnFunc2);
                return true;
            }
        }
    }
    return false;
}

bool CEngineModule::SetupModuleInfo()
{
    if (m_moduleHandle)
    {
        if (SysUtils::GetModuleInfo(GetCurrentProcess(), m_moduleHandle, m_moduleInfo)) {
            return true;
        }
    }
    return false;
}
