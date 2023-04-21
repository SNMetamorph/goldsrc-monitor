#pragma once
#include "hlsdk.h"
#include "sys_utils.h"
#include "build_info.h"
#include <stdint.h>

class CClientModule 
{
public:
    static CClientModule& GetInstance();

    bool FindHandle();
    bool FindEngfuncs(const CBuildInfo &buildInfo);
    uint8_t *GetFuncAddress(const char *funcName);
    inline ModuleHandle GetHandle() const   { return m_hModule;  }
    inline uint8_t *GetBaseAddress() const  { return m_ModuleInfo.baseAddress; }
    inline uint8_t *GetEntryPoint() const   { return m_ModuleInfo.entryPointAddress; }
    inline size_t   GetSize() const         { return m_ModuleInfo.imageSize; }

private:
    CClientModule();
    CClientModule(const CClientModule&) = delete;
    CClientModule& operator=(const CClientModule&) = delete;

    ModuleHandle m_hModule = NULL;
    SysUtils::ModuleInfo m_ModuleInfo;
};

extern CClientModule& g_ClientModule;
extern cl_enginefunc_t *g_pClientEngfuncs;

inline uint8_t *GetAddrCL(uint32_t offset)
{
    return g_ClientModule.GetBaseAddress() + offset;
}
