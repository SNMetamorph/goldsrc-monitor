#include "stdafx.h"
#include "core.h"
#include "util.h"
#include "exception.h"
#include "memory_defs.h"

#include <Windows.h>
#include <stdint.h>
#include <cstdio>

extern void apply_hooks();
int (*pfnGetBuildNumber)();
HMODULE g_hEngineModule;
HMODULE g_hClientModule;
HMODULE g_hServerModule;

playermove_t	*g_pPlayerMove;
cl_enginefunc_t *g_pClientEngFuncs;
enginefuncs_t	*g_pEngineFuncs;

void find_cl_engfuncs(uint8_t *module_addr, size_t module_size)
{
	void	*first_func_addr;
	void	*second_func_addr;
	uint8_t *coincidence_addr;

	first_func_addr = find_pattern_address(
		module_addr, module_size,
		SIGN_SPR_LOAD, MASK_SPR_LOAD
	);
	if (!first_func_addr)
		EXCEPT("not found SPR_Load() address");

	coincidence_addr = (uint8_t*)find_memory_value(
		(uint32_t*)module_addr,
		module_size,
		(uint32_t)first_func_addr
	);
	if (!coincidence_addr)
		EXCEPT("not found any pointers to SPR_Load()");

	uint8_t *probe_addr;
	probe_addr = *(uint8_t**)(coincidence_addr + sizeof(uint8_t*));
	second_func_addr = find_pattern_address(
		probe_addr, sizeof(SIGN_SPR_FRAMES) - 1, 
		SIGN_SPR_FRAMES, MASK_SPR_FRAMES
	);
	if (!second_func_addr)
		EXCEPT("not found SPR_Frames() address");

	g_pClientEngFuncs = (cl_enginefunc_t*)coincidence_addr;
}

void find_sv_engfuncs(uint8_t *module_addr, size_t module_size)
{
	void	*first_func_addr;
	void	*second_func_addr;
	uint8_t *coincidence_addr;

	first_func_addr = find_pattern_address(
		module_addr, module_size,
		SIGN_PRECACHE_MODEL, MASK_PRECACHE_MODEL
	);
	if (!first_func_addr)
		EXCEPT("not found PrecacheModel() address");

	coincidence_addr = (uint8_t*)find_memory_value(
		(uint32_t*)module_addr,
		module_size,
		(uint32_t)first_func_addr
	);
	if (!coincidence_addr)
		EXCEPT("not found any pointers to PrecacheModel()");

	uint8_t *probe_addr;
	probe_addr = *(uint8_t**)(coincidence_addr + sizeof(void*));
	second_func_addr = find_pattern_address(
		probe_addr, sizeof(SIGN_PRECACHE_SOUND) - 1,
		SIGN_PRECACHE_SOUND, MASK_PRECACHE_SOUND
	);
	if (!second_func_addr)
		EXCEPT("not found PrecacheSound() address");

	g_pEngineFuncs = (enginefuncs_t*)coincidence_addr;
}

void init_hacks()
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
	module_info_t engine_dll;
	get_module_info(GetCurrentProcess(), g_hEngineModule, engine_dll);
	pfnGetBuildNumber = (int(*)())find_pattern_address(
		engine_dll.base_addr,
		engine_dll.image_size,
		SIGN_BUILD_NUMBER,
		MASK_BUILD_NUMBER
	);
	if (!pfnGetBuildNumber)
	{
		pfnGetBuildNumber = (int(*)())find_pattern_address(
			engine_dll.base_addr,
			engine_dll.image_size,
			SIGN_BUILD_NUMBER_NEW,
			MASK_BUILD_NUMBER_NEW
		);
		if (!pfnGetBuildNumber)
			EXCEPT("not found get_build_number() address");
	}

	// find engine functions pointer arrays
	find_cl_engfuncs(engine_dll.base_addr, engine_dll.image_size);
	find_sv_engfuncs(engine_dll.base_addr, engine_dll.image_size);

	apply_hooks();
	init_stuff(engine_dll);
	show_intro_message();
}

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		try {
			init_hacks();
		}
		catch (CException ex)
		{
			snprintf(
				ex.message_buffer,
				sizeof(ex.message_buffer),
				"ERROR [%s:%d]: %s\nTry to run this tool already, or restart the game",
				ex.getFileName(),
				ex.getLineNumber(),
				ex.getDescription()
			);
			MessageBox(NULL, ex.message_buffer, "GoldSrc Monitor", MB_OK | MB_ICONWARNING);
			return FALSE;
		}
	}
	/*
	else if (nReason == DLL_PROCESS_DETACH)
		MessageBox(NULL, "Library unjected.", "", MB_OK);
	*/
	return TRUE;
}