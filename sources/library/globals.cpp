#pragma once
#include "stdafx.h"
#include <Windows.h>

HMODULE g_hEngineModule;
HMODULE g_hClientModule;
HMODULE g_hServerModule;

SCREENINFO          g_ScreenInfo;
playermove_t		*g_pPlayerMove;
cl_enginefunc_t	    *g_pClientEngFuncs;
enginefuncs_t	    *g_pEngineFuncs;
cvar_t              *gsm_color_r;
cvar_t              *gsm_color_g;
cvar_t              *gsm_color_b;
cvar_t              *gsm_mode;
