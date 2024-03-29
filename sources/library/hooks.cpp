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

#include "stdafx.h"
#include "hooks_impl.h"
#include "hooks.h"
#include "application.h"
#include "client_module.h"
#include "exception.h"
#include "local_player.h"

NOINLINE static int __cdecl HookRedraw(float time, int intermission)
{
    // call original function
    PLH::FnCast(CHooks::Impl::m_hookRedraw.GetTrampolineAddr(), CHooks::Impl::pfnRedraw_t())(time, intermission);
    g_Application.DisplayModeRender2D();
    return 1;
}

NOINLINE static void __cdecl HookPlayerMove(playermove_t *pmove, int server)
{
    PLH::FnCast(CHooks::Impl::m_hookPlayerMove.GetTrampolineAddr(), CHooks::Impl::pfnPlayerMove_t())(pmove, server);
    g_Application.GetLocalPlayer().UpdatePlayerMove(pmove);
}

NOINLINE static int __cdecl HookKeyEvent(int keyDown, int keyCode, const char *bindName)
{
    int returnCode = PLH::FnCast(CHooks::Impl::m_hookKeyEvent.GetTrampolineAddr(), CHooks::Impl::pfnKeyEvent_t())(
        keyDown, keyCode, bindName
    );
    return (returnCode && g_Application.KeyInput(keyDown, keyCode, bindName)) ? 1 : 0;
}

NOINLINE static void __cdecl HookDrawTriangles()
{
    PLH::FnCast(CHooks::Impl::m_hookDrawTriangles.GetTrampolineAddr(), CHooks::Impl::pfnDrawTriangles_t())();
    g_Application.DisplayModeRender3D();
}

NOINLINE static int __cdecl HookIsThirdPerson()
{
    int returnCode = PLH::FnCast(CHooks::Impl::m_hookIsThirdPerson.GetTrampolineAddr(), CHooks::Impl::pfnIsThirdPerson_t())();
    return (returnCode || g_Application.GetLocalPlayer().IsThirdPersonForced()) ? 1 : 0;
}

NOINLINE static void __cdecl HookCameraOffset(float *cameraOffset)
{
    PLH::FnCast(CHooks::Impl::m_hookCameraOffset.GetTrampolineAddr(), CHooks::Impl::pfnCameraOffset_t())(cameraOffset);
    if (g_Application.GetLocalPlayer().IsThirdPersonForced())
    {
        g_pClientEngfuncs->GetViewAngles(cameraOffset);
        cameraOffset[2] = g_Application.GetLocalPlayer().GetThirdPersonCameraDist();
    }
}

NOINLINE static int __cdecl HookVidInit()
{
    int returnCode = PLH::FnCast(CHooks::Impl::m_hookVidInit.GetTrampolineAddr(), CHooks::Impl::pfnVidInit_t())();
    g_Application.HandleChangelevel();
    return returnCode;
}

CHooks::CHooks(const CClientModule &moduleRef)
    : m_clientModule(moduleRef)
{
    m_pImpl = std::make_unique<CHooks::Impl>();
}

CHooks::~CHooks()
{
}

void CHooks::Apply()
{
    Impl::pfnRedraw_t pfnRedraw = (Impl::pfnRedraw_t)m_clientModule.GetFuncAddress("HUD_Redraw");
    Impl::pfnPlayerMove_t pfnPlayerMove = (Impl::pfnPlayerMove_t)m_clientModule.GetFuncAddress("HUD_PlayerMove");
    Impl::pfnKeyEvent_t pfnKeyEvent = (Impl::pfnKeyEvent_t)m_clientModule.GetFuncAddress("HUD_Key_Event");
    Impl::pfnDrawTriangles_t pfnDrawTriangles = (Impl::pfnDrawTriangles_t)m_clientModule.GetFuncAddress("HUD_DrawTransparentTriangles");
    Impl::pfnIsThirdPerson_t pfnIsThirdPerson = (Impl::pfnIsThirdPerson_t)m_clientModule.GetFuncAddress("CL_IsThirdPerson");
    Impl::pfnCameraOffset_t pfnCameraOffset = (Impl::pfnCameraOffset_t)m_clientModule.GetFuncAddress("CL_CameraOffset");
    Impl::pfnVidInit_t pfnVidInit = (Impl::pfnVidInit_t)m_clientModule.GetFuncAddress("HUD_VidInit");

    m_pImpl->InitializeLogger();
    Impl::m_hookRedraw.Hook(pfnRedraw, &HookRedraw);
    Impl::m_hookPlayerMove.Hook(pfnPlayerMove, &HookPlayerMove);
    Impl::m_hookKeyEvent.Hook(pfnKeyEvent, &HookKeyEvent);
    Impl::m_hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles);
    Impl::m_hookIsThirdPerson.Hook(pfnIsThirdPerson, &HookIsThirdPerson);
    Impl::m_hookCameraOffset.Hook(pfnCameraOffset, &HookCameraOffset);
    Impl::m_hookVidInit.Hook(pfnVidInit, &HookVidInit);

    if (!Impl::m_hookKeyEvent.IsHooked())
    {
        g_pClientEngfuncs->Con_Printf(
            "WARNING: KeyEvent() hooking failed: "
            "measurement mode will not react to keys.\n"
        );
    }

    if (!Impl::m_hookDrawTriangles.IsHooked())
    {
        pfnDrawTriangles = (Impl::pfnDrawTriangles_t)m_clientModule.GetFuncAddress("HUD_DrawNormalTriangles");
        if (!Impl::m_hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles))
        {
            g_pClientEngfuncs->Con_Printf(
                "WARNING: DrawTriangles() hooking failed: entity "
                "report mode will not draw entity hull.\n"
            );
        }
    }

    if (!Impl::m_hookIsThirdPerson.IsHooked() || !Impl::m_hookCameraOffset.IsHooked())
    {
        Impl::m_hookIsThirdPerson.Unhook();
        Impl::m_hookCameraOffset.Unhook();
        g_pClientEngfuncs->Con_Printf(
            "WARNING: IsThirdPerson() hooking failed: "
            "command gsm_thirdperson will not work.\n"
        );
    }

    bool isHookSuccessful = (
        Impl::m_hookRedraw.IsHooked() && 
        Impl::m_hookPlayerMove.IsHooked() &&
        Impl::m_hookVidInit.IsHooked()
    );
    if (!isHookSuccessful)
    {
        m_pImpl->RevertHooks();
        EXCEPT("unable to hook desired functions, check logs\n");
    }
}

void CHooks::Remove()
{
    // check for client.dll not already unloaded from process
    if (GetModuleHandle("client.dll")) {
        m_pImpl->RevertHooks();
    }
}
