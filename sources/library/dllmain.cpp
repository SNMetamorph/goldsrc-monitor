#include "stdafx.h"
#include "core.h"
#include "util.h"
#include "exception.h"
#include "buildinfo.h"
#include "hooks.h"

#include <Windows.h>
#include <stdint.h>
#include <cstdio>


static bool FindClientModule()
{
    g_hClientModule = FindModuleByExport(GetCurrentProcess(), "V_CalcRefdef");
    if (g_hClientModule)
        return true;
    else
        return false;
}


static void FindClientEngfuncs(uint8_t *moduleAddr, size_t moduleSize)
{
    void *pfnSPR_Load;
    void *pfnSPR_Frames;
    uint8_t *probeAddr;
    uint8_t *coincidenceAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleEndAddr;

    moduleEndAddr = moduleAddr + moduleSize;
    pfnSPR_Load = FindFunctionAddress(
        FUNCTYPE_SPR_LOAD, moduleAddr, moduleEndAddr
    );
    if (!pfnSPR_Load)
        EXCEPT("SPR_Load() address not found");

    scanStartAddr = moduleAddr;
    while (true)
    {
        coincidenceAddr = (uint8_t*)FindMemoryInt32(
            scanStartAddr,
            moduleEndAddr,
            (uint32_t)pfnSPR_Load
        );
        if (!coincidenceAddr || scanStartAddr >= moduleEndAddr)
            EXCEPT("valid pointer to SPR_Load() not found");
        else
            scanStartAddr = coincidenceAddr + sizeof(uint32_t);

        probeAddr = *(uint8_t**)(coincidenceAddr + sizeof(uint32_t));
        // check for module range to avoid segfault
        if (probeAddr >= moduleAddr && probeAddr < moduleEndAddr)
        {
            pfnSPR_Frames = FindFunctionAddress(FUNCTYPE_SPR_FRAMES, probeAddr);
            if (pfnSPR_Frames)
            {
                g_pClientEngFuncs = (cl_enginefunc_t*)coincidenceAddr;
                return;
            }
        }
    }
}


static void FindServerEngfuncs(uint8_t *moduleAddr, size_t moduleSize)
{
    void *pfnPrecacheModel;
    void *pfnPrecacheSound;
    uint8_t *probeAddr;
    uint8_t *coincidenceAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleEndAddr;

    moduleEndAddr = moduleAddr + moduleSize;
    pfnPrecacheModel = FindFunctionAddress(
        FUNCTYPE_PRECACHE_MODEL,
        moduleAddr,
        moduleEndAddr
    );
    if (!pfnPrecacheModel)
        EXCEPT("PrecacheModel() address not found");

    scanStartAddr = moduleAddr;
    while (true)
    {
        coincidenceAddr = (uint8_t*)FindMemoryInt32(
            scanStartAddr,
            moduleEndAddr,
            (uint32_t)pfnPrecacheModel
        );
        if (!coincidenceAddr || scanStartAddr >= moduleEndAddr)
            EXCEPT("valid pointer to PrecacheModel() not found");
        else
            scanStartAddr = coincidenceAddr + sizeof(uint32_t);

        probeAddr = *(uint8_t**)(coincidenceAddr + sizeof(uint32_t));
        if (probeAddr >= moduleAddr && probeAddr < moduleEndAddr)
        {
            pfnPrecacheSound = FindFunctionAddress(
                FUNCTYPE_PRECACHE_SOUND, probeAddr
            );
            if (pfnPrecacheSound)
            {
                g_pEngineFuncs = (enginefuncs_t*)coincidenceAddr;
                return;
            }
        }
    }
}


static void ProgramInit()
{
    // get module handles
    g_hEngineModule = GetModuleHandle("hw.dll");
    if (!g_hEngineModule)
        EXCEPT("failed to get engine module handle");

    if (!FindClientModule())
        EXCEPT("failed to get client module handle");

    // try to find GetBuildNumber() address
    moduleinfo_t engineDLL;
    GetModuleInfo(GetCurrentProcess(), g_hEngineModule, engineDLL);
    if (!FindBuildNumberFunc(engineDLL))
        EXCEPT("GetBuildNumber() address not found");

    // find engine functions pointer arrays
    FindClientEngfuncs(engineDLL.baseAddr, engineDLL.imageSize);
    FindServerEngfuncs(engineDLL.baseAddr, engineDLL.imageSize);

    ApplyHooks();
    SetupConVars(engineDLL);
    PrintTitleText();

    // load configuration file
    g_pClientEngFuncs->pfnClientCmd("exec gsm_config.cfg");
}


BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        try {
            ProgramInit();
        }
        catch (CException &ex)
        {
            snprintf(
                ex.m_szMessageBuffer,
                sizeof(ex.m_szMessageBuffer),
                "ERROR [%s:%d]: %s\nReport about error to the project page.\n"
                "Link: github.com/SNMetamorph/goldsrc-monitor/issues/1",
                ex.GetFileName(),
                ex.GetLineNumber(),
                ex.GetDescription()
            );
            MessageBox(NULL, ex.m_szMessageBuffer, "GoldSrc Monitor", MB_OK | MB_ICONWARNING);
            return FALSE;
        }
    }
    else if (nReason == DLL_PROCESS_DETACH)
    {
        RemoveHooks();
    }

    return TRUE;
}
