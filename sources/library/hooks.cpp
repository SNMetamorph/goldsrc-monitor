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
    g_LocalPlayer.UpdatePlayerMove(pmove);
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
    return (returnCode || g_LocalPlayer.IsThirdPersonForced()) ? 1 : 0;
}

NOINLINE static void __cdecl HookCameraOffset(float *cameraOffset)
{
    PLH::FnCast(CHooks::Impl::m_hookCameraOffset.GetTrampolineAddr(), CHooks::Impl::pfnCameraOffset_t())(cameraOffset);
    if (g_LocalPlayer.IsThirdPersonForced())
    {
        g_pClientEngfuncs->GetViewAngles(cameraOffset);
        cameraOffset[2] = g_LocalPlayer.GetThirdPersonCameraDist();
    }
}

NOINLINE static int __cdecl HookVidInit()
{
    int returnCode = PLH::FnCast(CHooks::Impl::m_hookVidInit.GetTrampolineAddr(), CHooks::Impl::pfnVidInit_t())();
    g_Application.HandleChangelevel();
    return returnCode;
}

CHooks::CHooks()
{
    m_pImpl = std::make_unique<CHooks::Impl>();
}

CHooks::~CHooks()
{
}

void CHooks::Apply()
{
    Impl::pfnRedraw_t pfnRedraw = (Impl::pfnRedraw_t)g_ClientModule.GetFuncAddress("HUD_Redraw");
    Impl::pfnPlayerMove_t pfnPlayerMove = (Impl::pfnPlayerMove_t)g_ClientModule.GetFuncAddress("HUD_PlayerMove");
    Impl::pfnKeyEvent_t pfnKeyEvent = (Impl::pfnKeyEvent_t)g_ClientModule.GetFuncAddress("HUD_Key_Event");
    Impl::pfnDrawTriangles_t pfnDrawTriangles = (Impl::pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawTransparentTriangles");
    Impl::pfnIsThirdPerson_t pfnIsThirdPerson = (Impl::pfnIsThirdPerson_t)g_ClientModule.GetFuncAddress("CL_IsThirdPerson");
    Impl::pfnCameraOffset_t pfnCameraOffset = (Impl::pfnCameraOffset_t)g_ClientModule.GetFuncAddress("CL_CameraOffset");
    Impl::pfnVidInit_t pfnVidInit = (Impl::pfnVidInit_t)g_ClientModule.GetFuncAddress("HUD_VidInit");

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
        pfnDrawTriangles = (Impl::pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawNormalTriangles");
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
        std::string errorLog;
        m_pImpl->WriteLogs(errorLog);
        m_pImpl->RevertHooks();
        EXCEPT("unable to hook desired functions, error log:\n" + errorLog);
    }
}

void CHooks::Remove()
{
    // check for client.dll not already unloaded from process
    if (GetModuleHandle("client.dll")) {
        m_pImpl->RevertHooks();
    }
}
