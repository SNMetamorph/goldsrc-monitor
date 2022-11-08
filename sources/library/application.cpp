#include "application.h"
#include "utils.h"
#include "hooks.h"
#include "cvars.h"
#include "app_info.h"
#include "exception.h"
#include "memory_pattern.h"
#include "engine_module.h"
#include "client_module.h"
#include "server_module.h"
#include "displaymode_full.h"
#include "displaymode_measurement.h"
#include "displaymode_speedometer.h"
#include "displaymode_entityreport.h"
#include "displaymode_angletracking.h"
#include "displaymode_facereport.h"
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
    
    InitializeConVars(engineDLL);
    SetCurrentDisplayMode();
    PrintTitleText();
    m_Hooks.Apply();

    // load configuration file
    g_pClientEngfuncs->pfnClientCmd("exec gsm_config.cfg");
}

void CApplication::InitializeDisplayModes()
{
    m_pDisplayModes.clear();
    m_pDisplayModes.push_back(std::make_shared<CModeFull>());
    m_pDisplayModes.push_back(std::make_shared<CModeSpeedometer>());
    m_pDisplayModes.push_back(std::make_shared<CModeEntityReport>());
    m_pDisplayModes.push_back(std::make_shared<CModeMeasurement>());
    m_pDisplayModes.push_back(std::make_shared<CModeFaceReport>());
    m_pDisplayModes.push_back(std::make_shared<CModeAngleTracking>());
}

void CApplication::HandleChangelevel()
{
    for (auto &mode : m_pDisplayModes) {
        mode->HandleChangelevel();
    }
}

void CApplication::FindTimescaleConVar(const ModuleInfo &engineLib)
{
    uint8_t *probeAddr;
    uint8_t *stringAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleStartAddr;
    uint8_t *moduleEndAddr;
    CMemoryPattern scanPattern("sys_timescale", 14);
    const size_t pointerSize = sizeof(void *);

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
        probeAddr = (uint8_t *)Utils::FindMemoryPointer(
            scanStartAddr, moduleEndAddr, stringAddr
        );

        if (!probeAddr || scanStartAddr >= moduleEndAddr)
            return;
        else
            scanStartAddr = probeAddr + pointerSize;

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
    g_pClientEngfuncs->Con_Printf(
        " \n"
        "   %s - utility for mapping/scripting/researching games on GoldSrc engine\n"
        "   Version : %d.%d\n"
        "   Compiled : %s\n"
        "   Link : %s\n"
        " \n"
        "  WARNING: This stuff is untested on VAC-secured\n"
        "  servers, therefore there is a risk to get VAC ban\n"
        "  while using it on VAC-secured servers.\n"
        " \n", 
        APP_TITLE_STR, 
        verMajor, verMinor, 
        APP_BUILD_DATE, 
        APP_GITHUB_LINK
    );
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
            float argument = static_cast<float>(std::atof(g_pClientEngfuncs->Cmd_Argv(1)));
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

void CApplication::InitializeConVars(ModuleInfo &engineLib)
{
    FindTimescaleConVar(engineLib);
    g_pClientEngfuncs->pfnAddCommand("gsm_timescale", &CommandTimescale);
    ConVars::gsm_color_r = Utils::RegisterConVar("gsm_color_r", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_color_g = Utils::RegisterConVar("gsm_color_g", "220", FCVAR_CLIENTDLL);
    ConVars::gsm_color_b = Utils::RegisterConVar("gsm_color_b", "220", FCVAR_CLIENTDLL);
    ConVars::gsm_margin_up = Utils::RegisterConVar("gsm_margin_up", "15", FCVAR_CLIENTDLL);
    ConVars::gsm_margin_right = Utils::RegisterConVar("gsm_margin_right", "400", FCVAR_CLIENTDLL);
    ConVars::gsm_mode = Utils::RegisterConVar("gsm_mode", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_debug = Utils::RegisterConVar("gsm_debug", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_thirdperson = Utils::RegisterConVar("gsm_thirdperson", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_thirdperson_dist = Utils::RegisterConVar("gsm_thirdperson_dist", "64", FCVAR_CLIENTDLL);
}

void CApplication::SetCurrentDisplayMode()
{
    DisplayModeIndex displayMode = Utils::GetCurrentDisplayMode();
    for (auto &mode : m_pDisplayModes) 
    {
        if (mode->GetModeIndex() == displayMode) 
        {
            m_pCurrentDisplayMode = mode;
            return;
        }
    }
    m_pCurrentDisplayMode = m_pDisplayModes[0];
}

void CApplication::UpdateScreenInfo()
{
    m_ScreenInfo.iSize = sizeof(m_ScreenInfo);
    g_pClientEngfuncs->pfnGetScreenInfo(&m_ScreenInfo);
}

// should be updated only once in frame
void CApplication::UpdateSmoothFrametime()
{
    const float smoothFactor    = 0.24f;
    const float diffThreshold   = 0.13f;
    float currentTime           = Utils::GetCurrentSysTime();
    float timeDelta             = currentTime - m_flLastClientTime;

    if ((timeDelta - m_flLastFrameTime) > diffThreshold)
        timeDelta = m_flLastFrameTime;

    m_flFrameTime       += (timeDelta - m_flFrameTime) * smoothFactor;
    m_flLastFrameTime   = m_flFrameTime;
    m_flLastClientTime  = currentTime;
}

void CApplication::DisplayModeRender2D()
{
    SetCurrentDisplayMode();
    UpdateSmoothFrametime();
    UpdateScreenInfo();
    m_pCurrentDisplayMode->Render2D(m_flFrameTime, m_ScreenInfo.iWidth, m_ScreenInfo.iHeight, m_StringStack);
}

void CApplication::DisplayModeRender3D()
{
    SetCurrentDisplayMode();
    m_pCurrentDisplayMode->Render3D();
}

bool CApplication::KeyInput(int keyDown, int keyCode, const char *bindName)
{
    return m_pCurrentDisplayMode->KeyInput(keyDown != 0, keyCode, bindName);
}
