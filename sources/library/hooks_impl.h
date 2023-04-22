/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#pragma once
#include "hooks.h"
#include "hlsdk.h"
#include "function_hook.h"

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
    void RevertHooks();

	std::shared_ptr<CHooks::Logger> m_pLogger;
	static CFunctionHook<pfnRedraw_t> m_hookRedraw;
	static CFunctionHook<pfnPlayerMove_t> m_hookPlayerMove;
	static CFunctionHook<pfnKeyEvent_t> m_hookKeyEvent;
	static CFunctionHook<pfnDrawTriangles_t> m_hookDrawTriangles;
	static CFunctionHook<pfnIsThirdPerson_t> m_hookIsThirdPerson;
	static CFunctionHook<pfnCameraOffset_t> m_hookCameraOffset;
	static CFunctionHook<pfnVidInit_t> m_hookVidInit;
};
