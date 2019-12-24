#include "core.h"
#include "util.h"
#include "drawing.h"
#include "app_version.h"
#include <stdint.h>

cvar_t *gsm_color_r;
cvar_t *gsm_color_g;
cvar_t *gsm_color_b;

static void cmd_timescale();
static cvar_t *sys_timescale;
static cvar_t *gsm_mode;
static SCREENINFO scr_info;

static cvar_t *RegisterCvar(const char *name, const char *value, int flags)
{
	cvar_t *probe = g_pClientEngFuncs->pfnGetCvarPointer(name);
	if (probe)
		return probe;
	return g_pClientEngFuncs->pfnRegisterVariable(name, value, flags);
}

static void FindTimescaleCvar(moduleinfo_t &engineLib)
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
            cvar_t *probeCvar   = (cvar_t*)probeAddr;
            uint8_t *stringAddr = (uint8_t*)probeCvar->string;
            if (stringAddr >= moduleStartAddr && stringAddr < moduleEndAddr)
            {
                if (strcmp(probeCvar->string, "1.0") == 0)
                {
                    sys_timescale = probeCvar;
                    return;
                }
            }
        }
    }
}

static bool FindServerModule(HMODULE &moduleHandle)
{
    moduleHandle = FindModuleByExport(GetCurrentProcess(), "GetEntityAPI");
    if (moduleHandle)
        return true;
    else
        return false;
}

void SetupCvars(moduleinfo_t &engineLib)
{
	scr_info.iSize = sizeof(scr_info);
	g_pClientEngFuncs->pfnGetScreenInfo(&scr_info);
	g_pEngineFuncs->pfnAddServerCommand("gsm_timescale", &cmd_timescale);

    FindTimescaleCvar(engineLib);
	gsm_color_r = RegisterCvar("gsm_color_r", "0", FCVAR_CLIENTDLL);
	gsm_color_g = RegisterCvar("gsm_color_g", "220", FCVAR_CLIENTDLL);
	gsm_color_b = RegisterCvar("gsm_color_b", "220", FCVAR_CLIENTDLL);
	gsm_mode	= RegisterCvar("gsm_mode", "0", FCVAR_CLIENTDLL);
}

void FrameDraw(float time, bool intermission, int screenWidth, int screenHeight)
{
    int displayMode = (int)gsm_mode->value;
    switch (displayMode)
    {
        case DISPLAYMODE_SPEEDOMETER:
            DrawModeSpeedometer(time, screenWidth, screenHeight);
            break;
        case DISPLAYMODE_ENTITYREPORT:
            DrawModeEntityReport(time, screenWidth, screenHeight);
            break;
        case DISPLAYMODE_ANGLETRACKING:
            DrawModeAngleTrack(time, screenWidth, screenHeight);
            break;
        default:
            DrawModeFull(time, screenWidth, screenHeight);
            break;
    }
}

int GetStringWidth(const char *str)
{
	int totalWidth = 0;
	for (char *i = (char*)str; *i; ++i)
		totalWidth += scr_info.charWidths[*i];
	return totalWidth;
}

void PrintTitleText()
{
    const int verMajor = APP_VERSION_MAJOR;
    const int verMinor = APP_VERSION_MINOR;

	g_pClientEngFuncs->Con_Printf(" \n");
	g_pClientEngFuncs->Con_Printf("   GoldScr Monitor | version %d.%d | by SNMetamorph  \n", 
        verMajor, verMinor);
	g_pClientEngFuncs->Con_Printf("         Debugging tool for GoldSrc-based games      \n");
	g_pClientEngFuncs->Con_Printf("   Use with caution, VAC can be react on this stuff  \n");
	g_pClientEngFuncs->Con_Printf(" \n");
	g_pClientEngFuncs->pfnClientCmd("play buttons/blip2.wav");
}

static void cmd_timescale()
{
    if (!sys_timescale)
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
                sys_timescale->value = argument;
                g_pClientEngFuncs->Con_Printf("sys_timescale value = %.1f\n", argument);
            }
            else
                g_pClientEngFuncs->Con_Printf("Value should be greater than zero\n");
        }
		else
			g_pClientEngFuncs->Con_Printf("Command using example: gsm_timescale 0.5\n");
	}
	else
		g_pClientEngFuncs->Con_Printf(
            "Server module not found! Start singleplayer "
            "or listen server and execute command again\n"
        );
}
