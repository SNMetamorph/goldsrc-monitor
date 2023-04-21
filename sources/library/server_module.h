#pragma once
#include "hlsdk.h"
#include "sys_utils.h"
#include "build_info.h"
#include <stdint.h>

class CServerModule 
{
public:
    static CServerModule& GetInstance();

    bool FindHandle();
    bool FindEngfuncs(const CBuildInfo &buildInfo);
    uint8_t *GetFuncAddress(const char *funcName);
    inline ModuleHandle GetHandle() const   { return m_hModule; }
    inline uint8_t *GetBaseAddress() const  { return m_ModuleInfo.baseAddress; }
    inline uint8_t *GetEntryPoint() const   { return m_ModuleInfo.entryPointAddress; }
    inline size_t   GetSize() const         { return m_ModuleInfo.imageSize; }
    inline bool     IsInitialized() const   { return m_hModule != NULL; }

private:
    CServerModule() {};
    CServerModule(const CServerModule&) = delete;
    CServerModule& operator=(const CServerModule&) = delete;

    ModuleHandle m_hModule = NULL;
    SysUtils::ModuleInfo m_ModuleInfo;
};

extern CServerModule &g_ServerModule;
extern enginefuncs_t *g_pServerEngfuncs;
