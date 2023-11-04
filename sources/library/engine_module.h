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
    bool IsXashEngine() const        { return m_isXashEngine; }
    bool IsSoftwareRenderer() const  { return m_isSoftwareRenderer; };
    ModuleHandle GetHandle() const   { return m_moduleHandle; };
    uint8_t* GetAddress() const      { return m_moduleInfo.baseAddress; };
    size_t GetSize() const           { return m_moduleInfo.imageSize; }

private:
    CEngineModule() {};
    CEngineModule(const CEngineModule&) = delete;
    CEngineModule& operator=(const CEngineModule&) = delete;
    bool SetupModuleInfo();

    ModuleHandle m_moduleHandle = NULL;
    SysUtils::ModuleInfo m_moduleInfo;
    bool m_isXashEngine = false;
    bool m_isSoftwareRenderer = false;
};
extern CEngineModule& g_EngineModule;
