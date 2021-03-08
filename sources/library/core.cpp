#include "core.h"
#include "util.h"
#include "hooks.h"
#include "globals.h"
#include "cvars.h"
#include "app_version.h"
#include "exception.h"
#include "buildinfo.h"
#include <stdint.h>
#include <gl/GL.h>

#include "displaymode_full.h"
#include "displaymode_measurement.h"
#include "displaymode_speedometer.h"
#include "displaymode_entityreport.h"
#include "displaymode_angletracking.h"


static bool FindServerModule(HMODULE &moduleHandle)
{
    moduleHandle = FindModuleByExport(GetCurrentProcess(), "GetEntityAPI");
    return moduleHandle != NULL;
}

static bool FindClientModule()
{
    g_hClientModule = FindModuleByExport(GetCurrentProcess(), "V_CalcRefdef");
    if (g_hClientModule)
        return true;
    else
        return false;
}

static bool FindEngineModule()
{
    g_hEngineModule = GetModuleHandle("hw.dll");
    if (!g_hEngineModule)
    {
        g_hEngineModule = GetModuleHandle("sw.dll");
        if (!g_hEngineModule)
            g_hEngineModule = GetModuleHandle("swds.dll");
    }
    return g_hEngineModule != NULL;
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

static cvar_t *RegisterConVar(const char *name, const char *value, int flags)
{
    cvar_t *probe = g_pClientEngFuncs->pfnGetCvarPointer(name);
    if (probe)
        return probe;
    return g_pClientEngFuncs->pfnRegisterVariable(name, value, flags);
}

static void FindTimescaleConVar(const moduleinfo_t &engineLib)
{
    uint8_t *probeAddr;
    uint8_t *stringAddr;
    uint8_t *scanStartAddr;
    uint8_t *moduleStartAddr;
    uint8_t *moduleEndAddr;
    const char *scanMask;
    size_t maskLength;

    moduleStartAddr = engineLib.baseAddr;
    moduleEndAddr   = moduleStartAddr + engineLib.imageSize;
    scanStartAddr   = moduleStartAddr;
    scanMask        = "xxxxxxxxxxxxx";
    maskLength      = strlen(scanMask);

    stringAddr = (uint8_t*)FindPatternAddress(
        scanStartAddr, moduleEndAddr,
        "sys_timescale", scanMask
    );
    if (!stringAddr)
        return;

    while (true)
    {
        probeAddr = (uint8_t*)FindMemoryInt32(
            scanStartAddr, moduleEndAddr, (uint32_t)stringAddr
        );

        if (!probeAddr || scanStartAddr >= moduleEndAddr)
            return;
        else
            scanStartAddr = probeAddr + sizeof(uint32_t);

        if (probeAddr >= moduleStartAddr && probeAddr < moduleEndAddr)
        {
            cvar_t *probeCvar = (cvar_t*)probeAddr;
            uint8_t *stringAddr = (uint8_t*)probeCvar->string;
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

static void CommandTimescale()
{
    if (!ConVars::sys_timescale)
    {
        g_pClientEngFuncs->Con_Printf("sys_timescale address not found");
        return;
    }

    if (g_hServerModule || FindServerModule(g_hServerModule))
    {
        if (g_pClientEngFuncs->Cmd_Argc() > 1)
        {
            float argument = (float)atof(g_pClientEngFuncs->Cmd_Argv(1));
            if (argument > 0.f)
            {
                ConVars::sys_timescale->value = argument;
                g_pClientEngFuncs->Con_Printf("sys_timescale value = %.1f\n", argument);
            }
            else
                g_pClientEngFuncs->Con_Printf("Value should be greater than zero\n");
        }
        else
            g_pClientEngFuncs->Con_Printf("Command using example: gsm_timescale 0.5\n");
    }
    else
    {
        g_pClientEngFuncs->Con_Printf(
            "Server module not found! Start singleplayer "
            "or listen server and execute command again\n"
        );
    }
}

static void PrintTitleText()
{
    const int verMajor = APP_VERSION_MAJOR;
    const int verMinor = APP_VERSION_MINOR;

    g_pClientEngFuncs->Con_Printf(" \n");
    g_pClientEngFuncs->Con_Printf("   GoldScr Monitor | version %d.%d | by SNMetamorph  \n",
        verMajor, verMinor);
    g_pClientEngFuncs->Con_Printf("         Debugging tool for GoldSrc-based games      \n");
    g_pClientEngFuncs->Con_Printf("   Use with caution, VAC can be react on this stuff  \n");
    g_pClientEngFuncs->Con_Printf(" \n");
    g_pClientEngFuncs->pfnPlaySoundByName("buttons/blip2.wav", 0.6f);
}

static void SetupConVars(moduleinfo_t &engineLib)
{
    g_ScreenInfo.iSize = sizeof(g_ScreenInfo);
    g_pClientEngFuncs->pfnGetScreenInfo(&g_ScreenInfo);
    g_pEngineFuncs->pfnAddServerCommand("gsm_timescale", &CommandTimescale);

    FindTimescaleConVar(engineLib);
    ConVars::gsm_color_r = RegisterConVar("gsm_color_r", "0", FCVAR_CLIENTDLL);
    ConVars::gsm_color_g = RegisterConVar("gsm_color_g", "220", FCVAR_CLIENTDLL);
    ConVars::gsm_color_b = RegisterConVar("gsm_color_b", "220", FCVAR_CLIENTDLL);
    ConVars::gsm_margin_up = RegisterConVar("gsm_margin_up", "15", FCVAR_CLIENTDLL);
    ConVars::gsm_margin_right = RegisterConVar("gsm_margin_right", "400", FCVAR_CLIENTDLL);
    ConVars::gsm_mode = RegisterConVar("gsm_mode", "0", FCVAR_CLIENTDLL);
}

void ProgramInit()
{
    if (!FindEngineModule())
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

    SetupConVars(engineDLL);
    AssignDisplayMode();
    PrintTitleText();
    ApplyHooks();

    // load configuration file
    g_pClientEngFuncs->pfnClientCmd("exec gsm_config.cfg");
}

void AssignDisplayMode()
{
    int displayMode = (int)ConVars::gsm_mode->value;
    switch (displayMode)
    {
    case DISPLAYMODE_SPEEDOMETER:
        g_pDisplayMode = &CModeSpeedometer::GetInstance();
        break;
    case DISPLAYMODE_ENTITYREPORT:
        g_pDisplayMode = &CModeEntityReport::GetInstance();
        break;
    case DISPLAYMODE_ANGLETRACKING:
        g_pDisplayMode = &CModeAngleTracking::GetInstance();
        break;
    case DISPLAYMODE_MEASUREMENT:
        g_pDisplayMode = &CModeMeasurement::GetInstance();
        break;
    default:
        g_pDisplayMode = &CModeFull::GetInstance();
        break;
    }
}

int GetStringWidth(const char *str)
{
    int totalWidth = 0;
    for (char *i = (char*)str; *i; ++i)
        totalWidth += g_ScreenInfo.charWidths[*i];
    return totalWidth;
}

void DrawStringStack(int marginRight, int marginUp, const CStringStack &stringStack)
{
    int linesSkipped = 0;
    int maxStringWidth = 0;
    int stringCount = stringStack.GetStringCount();
    const int stringHeight = 15;

    for (int i = 0; i < stringCount; ++i)
    {
        const char *textString = stringStack.StringAt(i);
        int stringWidth = GetStringWidth(textString);
        if (stringWidth > maxStringWidth)
            maxStringWidth = stringWidth;
    }

    for (int i = 0; i < stringCount; ++i)
    {
        const char *textString = stringStack.StringAt(i);
        g_pClientEngFuncs->pfnDrawString(
            g_ScreenInfo.iWidth - max(marginRight, maxStringWidth + 5),
            marginUp + (stringHeight * (i + linesSkipped)),
            textString,
            (int)ConVars::gsm_color_r->value,
            (int)ConVars::gsm_color_g->value,
            (int)ConVars::gsm_color_b->value
        );

        int lastCharIndex = strlen(textString) - 1;
        if (textString[lastCharIndex] == '\n')
            ++linesSkipped;
    }
}

bool IsSoftwareRenderer()
{
    static bool isChecked = false;
    static bool isSoftwareRenderer;

    if (!isChecked)
    {
        isSoftwareRenderer = GetModuleHandle("sw.dll") != nullptr;
        isChecked = true;
    }
        
    return isSoftwareRenderer;
}
