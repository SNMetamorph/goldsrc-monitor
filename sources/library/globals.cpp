#pragma once
#include "globals.h"

#define STRING_COUNT		32	
#define STRING_LENGTH		128
static char g_aStrings[STRING_COUNT][STRING_LENGTH];
CStringStack g_ScreenText(&g_aStrings[0][0], STRING_LENGTH, STRING_COUNT);

HMODULE g_hEngineModule;
HMODULE g_hClientModule;
HMODULE g_hServerModule;

SCREENINFO          g_ScreenInfo;
IDisplayMode        *g_pDisplayMode;
playermove_t		*g_pPlayerMove;
cl_enginefunc_t	    *g_pClientEngFuncs;
enginefuncs_t	    *g_pEngineFuncs;

