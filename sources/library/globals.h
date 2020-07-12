#pragma once
#include "stdafx.h"
#include "string_stack.h"
#include "display_mode.h"
#include <Windows.h> 

extern CStringStack g_ScreenText;
extern HMODULE g_hEngineModule;
extern HMODULE g_hClientModule;
extern HMODULE g_hServerModule;

extern SCREENINFO       g_ScreenInfo;
extern IDisplayMode     *g_pDisplayMode;
extern playermove_t		*g_pPlayerMove;
extern cl_enginefunc_t	*g_pClientEngFuncs;
extern enginefuncs_t	*g_pEngineFuncs;
extern cvar_t           *gsm_color_r;
extern cvar_t           *gsm_color_g;
extern cvar_t           *gsm_color_b;
extern cvar_t           *gsm_mode;
