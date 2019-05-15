#pragma once
#include "stdafx.h"
#include <Windows.h>

// export from dllmain.cpp
extern HMODULE g_hEngineModule;
extern HMODULE g_hClientModule;
extern HMODULE g_hServerModule;

extern playermove_t		*g_pPlayerMove;
extern cl_enginefunc_t	*g_pClientEngFuncs;
extern enginefuncs_t	*g_pEngineFuncs;
