#pragma once
#include "globals.h"

#define STRING_LENGTH		128
CStringStack g_ScreenText(STRING_LENGTH);

HMODULE g_hEngineModule;
HMODULE g_hClientModule;
HMODULE g_hServerModule;

SCREENINFO          g_ScreenInfo;
IDisplayMode        *g_pDisplayMode;
playermove_t		*g_pPlayerMove;
cl_enginefunc_t	    *g_pClientEngFuncs;
enginefuncs_t	    *g_pEngineFuncs;

