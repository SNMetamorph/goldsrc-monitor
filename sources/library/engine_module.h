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
#include <stdint.h>

class CEngineModule 
{
public:
    static CEngineModule& GetInstance();

    bool FindHandle();
    bool GetFunctionsFromAPI(uint8_t **pfnSPR_Load, uint8_t **pfnSPR_Frames);
    inline bool IsXashEngine() const        { return m_isXashEngine; }
    inline bool IsSoftwareRenderer() const  { return m_isSoftwareRenderer; };
    inline ModuleHandle GetHandle() const   { return m_hModule; };
    inline uint8_t* GetAddress() const      { return m_ModuleInfo.baseAddress; };
    inline size_t GetSize() const           { return m_ModuleInfo.imageSize; }

private:
    CEngineModule() {};
    CEngineModule(const CEngineModule&) = delete;
    CEngineModule& operator=(const CEngineModule&) = delete;
    bool SetupModuleInfo();

    ModuleHandle m_hModule = NULL;
    SysUtils::ModuleInfo m_ModuleInfo;
    bool m_isXashEngine = false;
    bool m_isSoftwareRenderer = false;
};
extern CEngineModule& g_EngineModule;
