#include "application.h"
#include "utils.h"
#include "hooks.h"
#include "cvars.h"
#include "app_version.h"
#include "exception.h"
#include "memory_pattern.h"
#include "engine_module.h"
#include "client_module.h"
#include "server_module.h"
#include <stdint.h>

CApplication &g_Application = CApplication::GetInstance();
CApplication &CApplication::GetInstance()
{
    static CApplication instance;
    return instance;
}

void CApplication::Run()
{
    if (!g_EngineModule.FindHandle())
        EXCEPT("failed to get engine module handle");

    if (!g_ClientModule.FindHandle())
        EXCEPT("failed to get client module handle");

    ModuleInfo engineDLL;
    Utils::GetModuleInfo(GetCurrentProcess(), g_EngineModule.GetHandle(), engineDLL);
    m_BuildInfo.Initialize(engineDLL);

    // find engine functions pointer arrays
    g_ClientModule.FindEngfuncs(m_BuildInfo);
    g_ServerModule.FindEngfuncs(m_BuildInfo);
    g_ServerModule.FindHandle();
    
    SetupConVars(engineDLL);
    AssignDisplayMode();
    PrintTitleText();
    m_Hooks.Apply();

    // load configuration file
    g_pClientEngfuncs->pfnClientCmd("exec gsm_config.cfg");
}

void CApplication::HandleChangelevel()
{
    // all display modes should be handled here
    m_ModeFull.HandleChangelevel();
    m_ModeSpeedometer.HandleChangelevel();
    m_ModeEntityReport.HandleChangelevel();
    m_ModeMeasurement.HandleChangelevel();
    m_ModeAngleTracking.HandleChangelevel();
}

void CApplication::FindTimescaleConVar(const ModuleInfo &engineLib)
{
    uint8_t *probeAddr;
    uint8_t *stringAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleStartAddr;
    uint8_t *moduleEndAddr;
    CMemoryPattern scanPattern("sys_timescale", 14);

    moduleStartAddr = engineLib.baseAddress;
    moduleEndAddr = moduleStartAddr + engineLib.imageSize;
    scanStartAddr = moduleStartAddr;
    stringAddr = (uint8_t *)Utils::FindPatternAddress(
        scanStartAddr, moduleEndAddr, scanPattern
    );
    if (!stringAddr)
        return;

    while (true)
    {
        probeAddr = (uint8_t *)Utils::FindMemoryInt32(
            scanStartAddr, moduleEndAddr, (uint32_t)stringAddr
        );

        if (!probeAddr || scanStartAddr >= moduleEndAddr)
            return;
        else
            scanStartAddr = probeAddr + sizeof(uint32_t);

        if (probeAddr >= moduleStartAddr && probeAddr < moduleEndAddr)
        {
            cvar_t *probeCvar = (cvar_t *)probeAddr;
            uint8_t *stringAddr = (uint8_t *)probeCvar->string;
            if (stringAddr >= moduleStartAddr && stringAddr < moduleEndAddr)
            {
                if (strcmp(probeCvar->string, "1.0") == 0)
                {
                    ConVars::sys_timescale = probeCvar;
                    return;
                }
            }
        }
    }
}

void CApplication::PrintTitleText()
{
    const int verMajor = APP_VERSION_MAJOR;
    const int verMinor = APP_VERSION_MINOR;

    g_pClientEngfuncs->Con_Printf(" \n");
    g_pClientEngfuncs->Con_Printf("   GoldScr Monitor | version %d.%d | by SNMetamorph  \n",
        verMajor, verMinor);
    g_pClientEngfuncs->Con_Printf("         Debugging tool for GoldSrc-based games      \n");
    g_pClientEngfuncs->Con_Printf("   Use with caution, VAC can be react on this stuff  \n");
    g_pClientEngfuncs->Con_Printf(" \n");
    g_pClientEngfuncs->pfnPlaySoundByName("buttons/blip2.wav", 0.6f);
}

static void CommandTimescale()
{
    if (!ConVars::sys_timescale)
    {
        g_pClientEngfuncs->Con_Printf("sys_timescale address not found");
        return;
    }

    if (g_ServerModule.IsInitialized() || g_ServerModule.FindHandle())
    {
        if (g_pClientEngfuncs->Cmd_Argc() > 1)
        {
            float argument = (float)atof(g_pClientEngfuncs->Cmd_Argv(1));
            if (argument > 0.f)
            {
                ConVars::sys_timescale->value = argument;
                g_pClientEngfuncs->Con_Printf("sys_timescale value = %.1f\n", argument);
            }
            else
                g_pClientEngfuncs->Con_Printf("Value should be greater than zero\n");
        }
        else
            g_pClientEngfuncs->Con_Printf("Command using example: gsm_timescale 0.5\n");
    }
    else
    {
        g_pClientEngfuncs->Con_Printf(
            "Server module not found! Start singleplayer "
            "or listen server and execute command again\n"
        );
    }
}

void CApplication::SetupConVars(ModuleInfo &engineLib)
{
    FindTimescaleConVar(engineLib);
    g_pClientEngfuncs->pfnAddCommand("gsm_timescale", &CommandTimescale);
    ConVars::gsm_color_r = Utils::RegisterConVar("gsm_color_r", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_color_g = Utils::RegisterConVar("gsm_color_g", "220", FCVAR_CLIENTDLL);
    ConVars::gsm_color_b = Utils::RegisterConVar("gsm_color_b", "220", FCVAR_CLIENTDLL);
    ConVars::gsm_margin_up = Utils::RegisterConVar("gsm_margin_up", "15", FCVAR_CLIENTDLL);
    ConVars::gsm_margin_right = Utils::RegisterConVar("gsm_margin_right", "400", FCVAR_CLIENTDLL);
    ConVars::gsm_mode = Utils::RegisterConVar("gsm_mode", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_thirdperson = Utils::RegisterConVar("gsm_thirdperson", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_thirdperson_dist = Utils::RegisterConVar("gsm_thirdperson_dist", "64", FCVAR_CLIENTDLL);
}

void CApplication::AssignDisplayMode()
{
    int displayMode = (int)ConVars::gsm_mode->value;
    switch (displayMode)
    {
    case DISPLAYMODE_SPEEDOMETER:
        m_pDisplayMode = &m_ModeSpeedometer;
        break;
    case DISPLAYMODE_ENTITYREPORT:
        m_pDisplayMode = &m_ModeEntityReport;
        break;
    case DISPLAYMODE_MEASUREMENT:
        m_pDisplayMode = &m_ModeMeasurement;
        break;
    case DISPLAYMODE_ANGLETRACKING:
        m_pDisplayMode = &m_ModeAngleTracking;
        break;
    default:
        m_pDisplayMode = &m_ModeFull;
        break;
    }
}

void CApplication::UpdateScreenInfo()
{
    m_ScreenInfo.iSize = sizeof(m_ScreenInfo);
    g_pClientEngfuncs->pfnGetScreenInfo(&m_ScreenInfo);
}

void CApplication::DisplayModeRender2D()
{
    AssignDisplayMode();
    UpdateScreenInfo();
    m_pDisplayMode->Render2D(m_ScreenInfo.iWidth, m_ScreenInfo.iHeight, m_StringStack);
}

void CApplication::DisplayModeRender3D()
{
    AssignDisplayMode();
    m_pDisplayMode->Render3D();
}

void CApplication::CheckForChangelevel(float currTime)
{
    static float oldTime = 3600.0f;
    if (currTime < oldTime)
        HandleChangelevel();
    oldTime = currTime;
}

bool CApplication::KeyInput(int keyDown, int keyCode, const char *bindName)
{
    return m_pDisplayMode->KeyInput(keyDown, keyCode, bindName);
}
