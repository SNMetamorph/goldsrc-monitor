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
#include "opengl_primitives_renderer.h"
#include <stdint.h>

//#define FILTER_AVG_EXPONENTIAL
#define FILTER_AVG_SIMPLE
#define FILTER_SIZE 5

CApplication &g_Application = CApplication::GetInstance();
CApplication &CApplication::GetInstance()
{
    static CApplication instance;
    return instance;
}

CApplication::CApplication()
    : m_clientModule(m_engineModule),
      m_serverModule(m_engineModule),
      m_hooks(m_clientModule),
      m_stringStack(128)
{
    InitializeDisplayModes();
    InitializePrimitivesRenderer();
};

void CApplication::Run()
{
    if (!m_engineModule.FindHandle())
        EXCEPT("failed to get engine module handle");

    if (!m_clientModule.FindHandle())
        EXCEPT("failed to get client module handle");

    SysUtils::ModuleInfo engineDLL;
    SysUtils::GetModuleInfo(SysUtils::GetCurrentProcessHandle(), m_engineModule.GetHandle(), engineDLL);
    
    m_buildInfo.Initialize(engineDLL);
    m_clientModule.FindEngfuncs(m_buildInfo);
    m_serverModule.FindEngfuncs(m_buildInfo);
    m_serverModule.FindHandle();
    
    InitializeConVars(engineDLL);
    SetCurrentDisplayMode();
    PrintTitleText();
    m_hooks.Apply();

    // load configuration file
    g_pClientEngfuncs->pfnClientCmd("exec gsm_config.cfg");
}

void CApplication::InitializeDisplayModes()
{
    m_displayModes.clear();
    m_displayModes.push_back(std::make_shared<CModeFull>(m_localPlayer));
    m_displayModes.push_back(std::make_shared<CModeSpeedometer>(m_localPlayer));
    m_displayModes.push_back(std::make_shared<CModeEntityReport>(m_localPlayer, m_engineModule));
    m_displayModes.push_back(std::make_shared<CModeMeasurement>(m_localPlayer));
    m_displayModes.push_back(std::make_shared<CModeFaceReport>(m_localPlayer));
    m_displayModes.push_back(std::make_shared<CModeAngleTracking>(m_localPlayer));
}

void CApplication::InitializePrimitivesRenderer()
{
    m_primitivesRenderer = std::make_shared<COpenGLPrimitivesRenderer>();
}

void CApplication::HandleChangelevel()
{
    for (auto &mode : m_displayModes) {
        mode->HandleChangelevel();
    }
}

void CApplication::FindTimescaleConVar(const SysUtils::ModuleInfo &engineLib)
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
    auto &serverModule = g_Application.GetServerModule();
    if (!ConVars::sys_timescale)
    {
        g_pClientEngfuncs->Con_Printf("sys_timescale address not found");
        return;
    }

    if (serverModule.IsInitialized() || serverModule.FindHandle())
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

void CApplication::InitializeConVars(const SysUtils::ModuleInfo &engineLib)
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
    DisplayModeType displayMode = Utils::GetCurrentDisplayMode();
    for (auto &mode : m_displayModes) 
    {
        if (mode->GetModeIndex() == displayMode) 
        {
            m_currentDisplayMode = mode;
            return;
        }
    }
    m_currentDisplayMode = m_displayModes[0];
}

void CApplication::UpdateScreenInfo()
{
    m_screenInfo.iSize = sizeof(m_screenInfo);
    g_pClientEngfuncs->pfnGetScreenInfo(&m_screenInfo);
}

// should be updated only once in frame
void CApplication::UpdateSmoothFrametime()
{
#ifdef FILTER_AVG_EXPONENTIAL
    // exponential running average filter
    // less k - more smooth result will be
    const float k               = 0.24f;
    const float diffThreshold   = 0.13f;
    float currentTime           = g_pClientEngfuncs->GetClientTime();
    float timeDelta             = currentTime - m_lastClientTime;

    if ((timeDelta - m_lastFrameTime) > diffThreshold)
        timeDelta = m_lastFrameTime;

    m_frameTime       += (timeDelta - m_frameTime) * k;
    m_lastFrameTime   = m_frameTime;
    m_lastClientTime  = currentTime;
#elif defined(FILTER_AVG_SIMPLE)
    float currentTime = g_pClientEngfuncs->GetClientTime();
    float timeDelta = currentTime - m_lastClientTime;
    static float buffer[FILTER_SIZE];
    double sum = 0.0;

    for (int i = FILTER_SIZE - 2; i >= 0; --i) {
        buffer[i + 1] = buffer[i];
    }

    buffer[0] = timeDelta;
    for (int i = 0; i < 5; ++i) {
        sum += buffer[i];
    }

    m_lastClientTime = currentTime;
    m_frameTime = sum / (double)FILTER_SIZE;
#else
    float currentTime = g_pClientEngfuncs->GetClientTime();
    float timeDelta = currentTime - m_lastClientTime;
    m_frameTime = timeDelta;
    m_lastClientTime = currentTime;
#endif
}

void CApplication::DisplayModeRender2D()
{
    SetCurrentDisplayMode();
    UpdateSmoothFrametime();
    UpdateScreenInfo();
    m_currentDisplayMode->Render2D(m_frameTime, m_screenInfo.iWidth, m_screenInfo.iHeight, m_stringStack);
}

void CApplication::DisplayModeRender3D()
{
    SetCurrentDisplayMode();
    m_currentDisplayMode->Render3D();
}

bool CApplication::KeyInput(int keyDown, int keyCode, const char *bindName)
{
    return m_currentDisplayMode->KeyInput(keyDown != 0, keyCode, bindName);
}
