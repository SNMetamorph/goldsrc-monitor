#include "core.h"
#include "util.h"
#include "memory_defs.h"
#include <stdint.h>

#define STRING_COUNT		17	
#define STRING_HEIGHT		15
#define STRING_LENGTH		128
#define STRING_MARGIN_RIGHT 400
#define STRING_MARGIN_UP	15
#define SPEEDOMETER_MARGIN	35

char g_aStrings[STRING_COUNT][STRING_LENGTH];

void cmd_timescale();
cvar_t *sys_timescale;
cvar_t *gsm_color_r;
cvar_t *gsm_color_g;
cvar_t *gsm_color_b;
cvar_t *gsm_mode;
SCREENINFO scr_info;

cvar_t *RegisterCvar(const char *name, const char *value, int flags)
{
	cvar_t *probe = g_pClientEngFuncs->pfnGetCvarPointer(name);
	if (probe)
		return probe;
	return g_pClientEngFuncs->pfnRegisterVariable(name, value, flags);
}

void SetupCvars(module_info_t &engine_lib)
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

void FrameDraw(float time, bool intermission, int scr_width, int scr_height)
{
	if (!g_pPlayerMove)
		return;

	if (gsm_mode->value == DISPLAYMODE_FULL)
	{
		sprintf_s(g_aStrings[0], STRING_LENGTH, "Velocity: %.2f u/s [X: %.2f, Y: %.2f, Z: %.2f]", g_pPlayerMove->velocity.Length2D(), g_pPlayerMove->velocity.x, g_pPlayerMove->velocity.y, g_pPlayerMove->velocity.z);
		sprintf_s(g_aStrings[1], STRING_LENGTH, "Origin: [%.2f, %.2f, %.2f]", g_pPlayerMove->origin.x, g_pPlayerMove->origin.y, g_pPlayerMove->origin.z);
		sprintf_s(g_aStrings[2], STRING_LENGTH, "Angles: [%.2f, %.2f, %.2f]", g_pPlayerMove->angles.x, g_pPlayerMove->angles.y, g_pPlayerMove->angles.z);
		sprintf_s(g_aStrings[3], STRING_LENGTH, "Base velocity: [%.2f, %.2f, %.2f]", g_pPlayerMove->basevelocity.x, g_pPlayerMove->basevelocity.y, g_pPlayerMove->basevelocity.z);
		sprintf_s(g_aStrings[4], STRING_LENGTH, "Punch angle: [%.2f, %.2f, %.2f]", g_pPlayerMove->punchangle.x, g_pPlayerMove->punchangle.y, g_pPlayerMove->punchangle.z);
		sprintf_s(g_aStrings[5], STRING_LENGTH, "View offset (Z axis): %.2f", g_pPlayerMove->view_ofs.z);
		sprintf_s(g_aStrings[6], STRING_LENGTH, "Texture name: %s", g_pPlayerMove->sztexturename);
		sprintf_s(g_aStrings[7], STRING_LENGTH, "Hull type: %d", g_pPlayerMove->usehull);
		sprintf_s(g_aStrings[8], STRING_LENGTH, "Gravity: %.2f", g_pPlayerMove->gravity);
		sprintf_s(g_aStrings[9], STRING_LENGTH, "Friction: %.2f", g_pPlayerMove->friction);
		sprintf_s(g_aStrings[10], STRING_LENGTH, "Max speed: %.2f / client %.2f", g_pPlayerMove->maxspeed, g_pPlayerMove->clientmaxspeed);
		sprintf_s(g_aStrings[11], STRING_LENGTH, "Flags: %x %x %x %x", g_pPlayerMove->flags >> 24, g_pPlayerMove->flags >> 16, g_pPlayerMove->flags >> 8, g_pPlayerMove->flags & 0xFF);
		sprintf_s(g_aStrings[12], STRING_LENGTH, "On ground: %s", g_pPlayerMove->onground != -1 ? "+" : "-");
		sprintf_s(g_aStrings[13], STRING_LENGTH, "Duck time/status: %.2f / %d", g_pPlayerMove->flDuckTime, g_pPlayerMove->bInDuck);
		sprintf_s(g_aStrings[14], STRING_LENGTH, "Time: %.3f seconds", time);
		sprintf_s(g_aStrings[15], STRING_LENGTH, "User variables (f): %.2f / %.2f / %.2f / %.2f", g_pPlayerMove->fuser1, g_pPlayerMove->fuser2, g_pPlayerMove->fuser3, g_pPlayerMove->fuser4);
		sprintf_s(g_aStrings[16], STRING_LENGTH, "User variables (i): %d / %d / %d / %d", g_pPlayerMove->iuser1, g_pPlayerMove->iuser2, g_pPlayerMove->iuser3, g_pPlayerMove->iuser4);

		for (int i = 0; i < STRING_COUNT; ++i)
		{
			g_pClientEngFuncs->pfnDrawString(
				scr_width - STRING_MARGIN_RIGHT, STRING_MARGIN_UP + (STRING_HEIGHT * i),
				g_aStrings[i], (int)gsm_color_r->value, (int)gsm_color_g->value, (int)gsm_color_b->value
			);
		}
	}
	else if (gsm_mode->value == DISPLAYMODE_SPEEDOMETER)
	{
		int string_width;
		float player_speed = g_pPlayerMove->velocity.Length2D();
		sprintf_s(g_aStrings[0], STRING_LENGTH, "%.2f", player_speed);
		string_width = GetStringWidth(g_aStrings[0]);

		g_pClientEngFuncs->pfnDrawString(
			(scr_width / 2) - (string_width / 2), (scr_height / 2) + SPEEDOMETER_MARGIN, g_aStrings[0],
			(int)gsm_color_r->value, (int)gsm_color_g->value, (int)gsm_color_b->value
		);
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

void cmd_timescale()
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
