#include "stdafx.h"
#include "hooks.h"
#include "application.h"
#include "client_module.h"
#include "exception.h"
#include "function_hook.h"
#include "local_player.h"

// hooking stuff
typedef int (__cdecl *pfnRedraw_t)(float, int);
typedef void (__cdecl *pfnPlayerMove_t)(playermove_t*, int);
typedef int (__cdecl *pfnKeyEvent_t)(int, int, const char *);
typedef void (__cdecl *pfnDrawTriangles_t)();
typedef int (__cdecl *pfnIsThirdPerson_t)();
typedef void (__cdecl *pfnCameraOffset_t)(float *);

CFunctionHook<pfnRedraw_t> hookRedraw;
CFunctionHook<pfnPlayerMove_t> hookPlayerMove;
CFunctionHook<pfnKeyEvent_t> hookKeyEvent;
CFunctionHook<pfnDrawTriangles_t> hookDrawTriangles;
CFunctionHook<pfnIsThirdPerson_t> hookIsThirdPerson;
CFunctionHook<pfnCameraOffset_t> hookCameraOffset;

NOINLINE static int __cdecl HookRedraw(float time, int intermission)
{
    // call original function
    PLH::FnCast(hookRedraw.GetTrampolineAddr(), pfnRedraw_t())(time, intermission);
    g_Application.CheckForChangelevel(time);
    if (g_pPlayerMove)
    {
        bool isIntermission = intermission != 0;
        g_Application.DisplayModeRender2D();
    }
    return 1;
}

NOINLINE static void __cdecl HookPlayerMove(playermove_t *pmove, int server)
{
    PLH::FnCast(hookPlayerMove.GetTrampolineAddr(), pfnPlayerMove_t())(pmove, server);
    g_LocalPlayer.Setup(pmove);
}

NOINLINE static int __cdecl HookKeyEvent(int keyDown, int keyCode, const char *bindName)
{
    int returnCode = PLH::FnCast(hookKeyEvent.GetTrampolineAddr(), pfnKeyEvent_t())(
        keyDown, keyCode, bindName
        );
    return returnCode && g_Application.KeyInput(keyDown, keyCode, bindName);
}

NOINLINE static void __cdecl HookDrawTriangles()
{
    PLH::FnCast(hookDrawTriangles.GetTrampolineAddr(), pfnDrawTriangles_t())();
    g_Application.DisplayModeRender3D();
}

NOINLINE static int __cdecl HookIsThirdPerson()
{
    int returnCode = PLH::FnCast(hookIsThirdPerson.GetTrampolineAddr(), pfnIsThirdPerson_t())();
    return returnCode || g_LocalPlayer.IsThirdPersonForced();
}

NOINLINE static void __cdecl HookCameraOffset(float *cameraOffset)
{
    PLH::FnCast(hookCameraOffset.GetTrampolineAddr(), pfnCameraOffset_t())(cameraOffset);
    if (g_LocalPlayer.IsThirdPersonForced())
    {
        g_pClientEngfuncs->GetViewAngles(cameraOffset);
        cameraOffset[2] = g_LocalPlayer.GetThirdPersonCameraDist();
    }
}

static void DisposeHooks()
{
    hookRedraw.Unhook();
    hookPlayerMove.Unhook();
    hookKeyEvent.Unhook();
    hookDrawTriangles.Unhook();
    hookIsThirdPerson.Unhook();
    hookCameraOffset.Unhook();
}

void ÑHooks::Apply()
{
    pfnRedraw_t pfnRedraw = (pfnRedraw_t)g_ClientModule.GetFuncAddress("HUD_Redraw");
    pfnPlayerMove_t pfnPlayerMove = (pfnPlayerMove_t)g_ClientModule.GetFuncAddress("HUD_PlayerMove");
    pfnKeyEvent_t pfnKeyEvent = (pfnKeyEvent_t)g_ClientModule.GetFuncAddress("HUD_Key_Event");
    pfnDrawTriangles_t pfnDrawTriangles = (pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawTransparentTriangles");
    pfnIsThirdPerson_t pfnIsThirdPerson = (pfnIsThirdPerson_t)g_ClientModule.GetFuncAddress("CL_IsThirdPerson");
    pfnCameraOffset_t pfnCameraOffset = (pfnCameraOffset_t)g_ClientModule.GetFuncAddress("CL_CameraOffset");

    hookRedraw.Hook(pfnRedraw, &HookRedraw);
    hookPlayerMove.Hook(pfnPlayerMove, &HookPlayerMove);
    hookKeyEvent.Hook(pfnKeyEvent, &HookKeyEvent);
    hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles);
    hookIsThirdPerson.Hook(pfnIsThirdPerson, &HookIsThirdPerson);
    hookCameraOffset.Hook(pfnCameraOffset, &HookCameraOffset);

    if (!hookKeyEvent.IsHooked())
    {
        g_pClientEngfuncs->Con_Printf(
            "WARNING: KeyEvent() hooking failed: "
            "measurement mode will not react to keys."
        );
    }

    if (!hookDrawTriangles.IsHooked())
    {
        pfnDrawTriangles_t pfnDrawTriangles = (pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawNormalTriangles");
        if (!hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles))
        {
            g_pClientEngfuncs->Con_Printf(
                "WARNING: DrawTriangles() hooking failed: entity "
                "report mode will not draw entity hull lines."
            );
        }
    }

    if (!hookIsThirdPerson.IsHooked() || !hookCameraOffset.IsHooked())
    {
        hookIsThirdPerson.Unhook();
        hookCameraOffset.Unhook();
        g_pClientEngfuncs->Con_Printf(
            "WARNING: IsThirdPerson() hooking failed: "
            "command gsm_thirdperson will be unavailable."
        );
    }

    bool isHookSuccessful = hookRedraw.IsHooked() && hookPlayerMove.IsHooked();
    if (!isHookSuccessful)
    {
        DisposeHooks();
        EXCEPT("unable to hook desired functions");
    }
}

void ÑHooks::Remove()
{
    // check for client.dll not already unloaded from process
    if (GetModuleHandle("client.dll"))
    {
        DisposeHooks();
    }
}
