#include "server_module.h"
#include "engine_module.h"
#include "build_info.h"
#include "exception.h"
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
    HANDLE currProcess = GetCurrentProcess();
    m_hModule = Utils::FindModuleByExport(GetCurrentProcess(), "GetEntityAPI");
    Utils::GetModuleInfo(currProcess, m_hModule, m_ModuleInfo);
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

    moduleAddr = g_EngineModule.GetAddress();
    moduleEndAddr = moduleAddr + g_EngineModule.GetSize();

    const CBuildInfoEntry &buildInfoEntry = buildInfo.GetInfoEntry();
    if (buildInfoEntry.HasServerEngfuncsOffset()) {
        g_pServerEngfuncs = (enginefuncs_t *)(moduleAddr + buildInfoEntry.GetServerEngfuncsOffset());
        return true;
    }

    pfnPrecacheModel = buildInfo.FindFunctionAddress(
        FUNCTYPE_PRECACHE_MODEL,
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
        coincidenceAddr = (uint8_t *)Utils::FindMemory(
            scanStartAddr,
            moduleEndAddr,
            (uint32_t)pfnPrecacheModel
        );
        if (!coincidenceAddr || scanStartAddr >= moduleEndAddr) 
        {
            // valid pointer to PrecacheModel() not found
            return false;
        }
        else
            scanStartAddr = coincidenceAddr + sizeof(uint32_t);

        probeAddr = *(uint8_t **)(coincidenceAddr + sizeof(uint32_t));
        if (probeAddr >= moduleAddr && probeAddr < moduleEndAddr)
        {
            pfnPrecacheSound = buildInfo.FindFunctionAddress(
                FUNCTYPE_PRECACHE_SOUND, probeAddr
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
