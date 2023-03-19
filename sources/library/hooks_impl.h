#pragma once
#include "hooks.h"
#include "hlsdk.h"
#include "function_hook.h"
#include <polyhook2/ErrorLog.hpp>

class CHooks::Impl
{
public:
	typedef int(__cdecl *pfnRedraw_t)(float, int);
    typedef void(__cdecl *pfnPlayerMove_t)(playermove_t *, int);
    typedef int(__cdecl *pfnKeyEvent_t)(int, int, const char *);
    typedef void(__cdecl *pfnDrawTriangles_t)();
    typedef int(__cdecl *pfnIsThirdPerson_t)();
    typedef void(__cdecl *pfnCameraOffset_t)(float *);
    typedef int(__cdecl *pfnVidInit_t)();

	void InitializeLogger();
    void WriteLogs(std::string &errorLog);
    void RevertHooks();

	std::shared_ptr<PLH::ErrorLog> m_pLogger;
	static CFunctionHook<pfnRedraw_t> m_hookRedraw;
	static CFunctionHook<pfnPlayerMove_t> m_hookPlayerMove;
	static CFunctionHook<pfnKeyEvent_t> m_hookKeyEvent;
	static CFunctionHook<pfnDrawTriangles_t> m_hookDrawTriangles;
	static CFunctionHook<pfnIsThirdPerson_t> m_hookIsThirdPerson;
	static CFunctionHook<pfnCameraOffset_t> m_hookCameraOffset;
	static CFunctionHook<pfnVidInit_t> m_hookVidInit;
};
