#include "core.h"
#include "util.h"
#include "drawing.h"
#include "memory_defs.h"
#include <stdint.h>

cvar_t *gsm_color_r;
cvar_t *gsm_color_g;
cvar_t *gsm_color_b;

static void cmd_timescale();
static cvar_t *sys_timescale;
static cvar_t *gsm_mode;
static SCREENINFO scr_info;

cvar_t *RegisterCvar(const char *name, const char *value, int flags)
{
	cvar_t *probe = g_pClientEngFuncs->pfnGetCvarPointer(name);
	if (probe)
		return probe;
	return g_pClientEngFuncs->pfnRegisterVariable(name, value, flags);
}

void SetupCvars(moduleinfo_t &engine_lib)
{
	scr_info.iSize = sizeof(scr_info);
	g_pClientEngFuncs->pfnGetScreenInfo(&scr_info);
	g_pEngineFuncs->pfnAddServerCommand("gsm_timescale", &cmd_timescale);

	sys_timescale = (cvar_t*)((uint8_t*)g_pEngineFuncs->pfnCVarGetPointer("fps_max") - OFFSET_TIMESCALE);
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
	g_pClientEngFuncs->Con_Printf("###################################\n");
	g_pClientEngFuncs->Con_Printf("#  GoldScr Monitor | version 1.1 | by SNMetamorph    \n");
	g_pClientEngFuncs->Con_Printf("#       Debugging tool for GoldSrc-based games		\n");
	g_pClientEngFuncs->Con_Printf("#  Use with caution, VAC can be react on this stuff.  \n");
	g_pClientEngFuncs->Con_Printf("###################################\n");
	g_pClientEngFuncs->pfnClientCmd("play buttons/blip2.wav");
}

static void cmd_timescale()
{
	if (!g_hServerModule)
		FindServerModule(g_hServerModule);

	if (g_hServerModule)
	{
		if (g_pClientEngFuncs->Cmd_Argc() > 1)
			sys_timescale->value = (float)atof(g_pClientEngFuncs->Cmd_Argv(1));
		else
			g_pClientEngFuncs->pfnConsolePrint("Invalid syntax, using example: timescale 0.5\n");
	}
	else
		g_pClientEngFuncs->pfnConsolePrint("Server module not found! Start listen server and execute command again.\n");
}
