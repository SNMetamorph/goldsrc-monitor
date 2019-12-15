#include "stdafx.h"
#include "core.h"
#include "util.h"
#include "exception.h"
#include "memory_defs.h"

#include <Windows.h>
#include <stdint.h>
#include <cstdio>

extern void ApplyHooks();
extern void RemoveHooks();
extern int (*pfnGetBuildNumber)();
HMODULE g_hEngineModule;
HMODULE g_hClientModule;
HMODULE g_hServerModule;

playermove_t	*g_pPlayerMove;
enginefuncs_t	*g_pEngineFuncs;
cl_enginefunc_t *g_pClientEngFuncs;

void FindClientEngfuncs(uint8_t *module_addr, size_t module_size)
{
	void	*first_func_addr;
	void	*second_func_addr;
	uint8_t *coincidence_addr;

	first_func_addr = FindPatternAddress(
		module_addr, module_size,
		SIGN_SPR_LOAD, MASK_SPR_LOAD
	);
	if (!first_func_addr)
		EXCEPT("SPR_Load() address not found");

	coincidence_addr = (uint8_t*)FindMemoryValue(
		(uint32_t*)module_addr,
		module_size,
		(uint32_t)first_func_addr
	);
	if (!coincidence_addr)
		EXCEPT("not found any pointers to SPR_Load()");

	uint8_t *probe_addr;
	probe_addr = *(uint8_t**)(coincidence_addr + sizeof(uint8_t*));
	second_func_addr = FindPatternAddress(
		probe_addr, sizeof(SIGN_SPR_FRAMES) - 1, 
		SIGN_SPR_FRAMES, MASK_SPR_FRAMES
	);
	if (!second_func_addr)
		EXCEPT("SPR_Frames() address not found");

	g_pClientEngFuncs = (cl_enginefunc_t*)coincidence_addr;
}

void FindServerEngfuncs(uint8_t *module_addr, size_t module_size)
{
	void	*first_func_addr;
	void	*second_func_addr;
	uint8_t *coincidence_addr;

	first_func_addr = FindPatternAddress(
		module_addr, module_size,
		SIGN_PRECACHE_MODEL, MASK_PRECACHE_MODEL
	);
	if (!first_func_addr)
		EXCEPT("PrecacheModel() address not found");

	coincidence_addr = (uint8_t*)FindMemoryValue(
		(uint32_t*)module_addr,
		module_size,
		(uint32_t)first_func_addr
	);
	if (!coincidence_addr)
		EXCEPT("not found any pointers to PrecacheModel()");

	uint8_t *probe_addr;
	probe_addr = *(uint8_t**)(coincidence_addr + sizeof(void*));
	second_func_addr = FindPatternAddress(
		probe_addr, sizeof(SIGN_PRECACHE_SOUND) - 1,
		SIGN_PRECACHE_SOUND, MASK_PRECACHE_SOUND
	);
	if (!second_func_addr)
		EXCEPT("PrecacheSound() address not found");

	g_pEngineFuncs = (enginefuncs_t*)coincidence_addr;
}

void ProgramInit()
{
	// get module handles
	g_hEngineModule = GetModuleHandle("hw.dll");
	if (!g_hEngineModule)
		EXCEPT("failed to get engine module handle");

	g_hClientModule = GetModuleHandle("client.dll");
	if (!g_hClientModule)
		EXCEPT("failed to get client module handle");

	g_hServerModule = GetModuleHandle("mp.dll");
	if (!g_hServerModule)
		g_hServerModule = GetModuleHandle("hl.dll");

	// find get_build_number() address
	module_info_t engineDLL;
	GetModuleInfo(GetCurrentProcess(), g_hEngineModule, engineDLL);
	pfnGetBuildNumber = (int(*)())FindPatternAddress(
		engineDLL.baseAddr,
		engineDLL.imageSize,
		SIGN_BUILD_NUMBER,
		MASK_BUILD_NUMBER
	);

	if (!pfnGetBuildNumber)
	{
		pfnGetBuildNumber = (int(*)())FindPatternAddress(
			engineDLL.baseAddr,
			engineDLL.imageSize,
			SIGN_BUILD_NUMBER_NEW,
			MASK_BUILD_NUMBER_NEW
		);
		if (!pfnGetBuildNumber)
			EXCEPT("GetBuildNumber() address not found");
	}

	// find engine functions pointer arrays
	FindClientEngfuncs(engineDLL.baseAddr, engineDLL.imageSize);
	FindServerEngfuncs(engineDLL.baseAddr, engineDLL.imageSize);

	ApplyHooks();
	SetupCvars(engineDLL);
	PrintTitleText();
}

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		try {
			ProgramInit();
		}
		catch (CException &ex)
		{
			snprintf(
				ex.m_szMessageBuffer,
				sizeof(ex.m_szMessageBuffer),
				"ERROR [%s:%d]: %s\nReport about error to the project page.\n"
				"Link: github.com/SNMetamorph/goldsrc-monitor/issues/1",
				ex.GetFileName(),
				ex.GetLineNumber(),
				ex.GetDescription()
			);
			MessageBox(NULL, ex.m_szMessageBuffer, "GoldSrc Monitor", MB_OK | MB_ICONWARNING);
			return FALSE;
		}
	}
	else if (nReason == DLL_PROCESS_DETACH)
	{
		RemoveHooks();
	}

	return TRUE;
}