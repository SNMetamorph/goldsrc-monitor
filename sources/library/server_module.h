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
#include "hlsdk.h"
#include "sys_utils.h"
#include "build_info.h"
#include "engine_module.h"
#include <stdint.h>

class CServerModule 
{
public:
    CServerModule(const CEngineModule &moduleRef);

    bool FindHandle();
    bool FindEngfuncs(const CBuildInfo &buildInfo);
    uint8_t *GetFuncAddress(const char *funcName);
    ModuleHandle GetHandle() const   { return m_module; }
    uint8_t *GetBaseAddress() const  { return m_moduleInfo.baseAddress; }
    uint8_t *GetEntryPoint() const   { return m_moduleInfo.entryPointAddress; }
    size_t   GetSize() const         { return m_moduleInfo.imageSize; }
    bool     IsInitialized() const   { return m_module != NULL; }

private:
    ModuleHandle m_module = NULL;
    SysUtils::ModuleInfo m_moduleInfo;
    const CEngineModule &m_engineModule;
};

extern enginefuncs_t *g_pServerEngfuncs;
