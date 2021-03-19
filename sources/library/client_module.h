#pragma once
#include "hlsdk.h"
#include "moduleinfo.h"
#include <windows.h>
#include <stdint.h>

class CClientModule 
{
public:
    static CClientModule& GetInstance();

    bool FindHandle();
    bool FindEngfuncs();
    uint8_t *GetFuncAddress(const char *funcName);
    inline HMODULE  GetHandle() const       { return m_hModule;  }
    inline uint8_t *GetBaseAddress() const  { return m_ModuleInfo.baseAddr; }
    inline uint8_t *GetEntryPoint() const   { return m_ModuleInfo.entryPointAddr; }
    inline size_t   GetSize() const         { return m_ModuleInfo.imageSize; }

private:
    CClientModule() {};
    CClientModule(const CClientModule&) = delete;
    CClientModule& operator=(const CClientModule&) = delete;

    HMODULE         m_hModule = NULL;
    moduleinfo_t    m_ModuleInfo;
};

extern CClientModule& g_ClientModule;
extern cl_enginefunc_t *g_pClientEngfuncs;
extern playermove_t *g_pPlayerMove;

inline uint8_t *GetAddrCL(uint32_t offset)
{
    return g_ClientModule.GetBaseAddress() + offset;
}
