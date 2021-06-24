#include "stdafx.h"
#include "hooks.h"
#include "application.h"
#include "client_module.h"
#include "exception.h"
#include "local_player.h"

static CFunctionHook<CHooks::pfnRedraw_t> g_hookRedraw;
static CFunctionHook<CHooks::pfnPlayerMove_t> g_hookPlayerMove;
static CFunctionHook<CHooks::pfnKeyEvent_t> g_hookKeyEvent;
static CFunctionHook<CHooks::pfnDrawTriangles_t> g_hookDrawTriangles;
static CFunctionHook<CHooks::pfnIsThirdPerson_t> g_hookIsThirdPerson;
static CFunctionHook<CHooks::pfnCameraOffset_t> g_hookCameraOffset;

NOINLINE static int __cdecl HookRedraw(float time, int intermission)
{
    // call original function
    PLH::FnCast(g_hookRedraw.GetTrampolineAddr(), CHooks::pfnRedraw_t())(time, intermission);
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
    PLH::FnCast(g_hookPlayerMove.GetTrampolineAddr(), CHooks::pfnPlayerMove_t())(pmove, server);
    g_LocalPlayer.Setup(pmove);
}

NOINLINE static int __cdecl HookKeyEvent(int keyDown, int keyCode, const char *bindName)
{
    int returnCode = PLH::FnCast(g_hookKeyEvent.GetTrampolineAddr(), CHooks::pfnKeyEvent_t())(
        keyDown, keyCode, bindName
    );
    return returnCode && g_Application.KeyInput(keyDown, keyCode, bindName);
}

NOINLINE static void __cdecl HookDrawTriangles()
{
    PLH::FnCast(g_hookDrawTriangles.GetTrampolineAddr(), CHooks::pfnDrawTriangles_t())();
    g_Application.DisplayModeRender3D();
}

NOINLINE static int __cdecl HookIsThirdPerson()
{
    int returnCode = PLH::FnCast(g_hookIsThirdPerson.GetTrampolineAddr(), CHooks::pfnIsThirdPerson_t())();
    return returnCode || g_LocalPlayer.IsThirdPersonForced();
}

NOINLINE static void __cdecl HookCameraOffset(float *cameraOffset)
{
    PLH::FnCast(g_hookCameraOffset.GetTrampolineAddr(), CHooks::pfnCameraOffset_t())(cameraOffset);
    if (g_LocalPlayer.IsThirdPersonForced())
    {
        g_pClientEngfuncs->GetViewAngles(cameraOffset);
        cameraOffset[2] = g_LocalPlayer.GetThirdPersonCameraDist();
    }
}

void CHooks::Apply()
{
    pfnRedraw_t pfnRedraw = (pfnRedraw_t)g_ClientModule.GetFuncAddress("HUD_Redraw");
    pfnPlayerMove_t pfnPlayerMove = (pfnPlayerMove_t)g_ClientModule.GetFuncAddress("HUD_PlayerMove");
    pfnKeyEvent_t pfnKeyEvent = (pfnKeyEvent_t)g_ClientModule.GetFuncAddress("HUD_Key_Event");
    pfnDrawTriangles_t pfnDrawTriangles = (pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawTransparentTriangles");
    pfnIsThirdPerson_t pfnIsThirdPerson = (pfnIsThirdPerson_t)g_ClientModule.GetFuncAddress("CL_IsThirdPerson");
    pfnCameraOffset_t pfnCameraOffset = (pfnCameraOffset_t)g_ClientModule.GetFuncAddress("CL_CameraOffset");

    g_hookRedraw.Hook(pfnRedraw, &HookRedraw);
    g_hookPlayerMove.Hook(pfnPlayerMove, &HookPlayerMove);
    g_hookKeyEvent.Hook(pfnKeyEvent, &HookKeyEvent);
    g_hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles);
    g_hookIsThirdPerson.Hook(pfnIsThirdPerson, &HookIsThirdPerson);
    g_hookCameraOffset.Hook(pfnCameraOffset, &HookCameraOffset);

    if (!g_hookKeyEvent.IsHooked())
    {
        g_pClientEngfuncs->Con_Printf(
            "WARNING: KeyEvent() hooking failed: "
            "measurement mode will not react to keys."
        );
    }

    if (!g_hookDrawTriangles.IsHooked())
    {
        pfnDrawTriangles_t pfnDrawTriangles = (pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawNormalTriangles");
        if (!g_hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles))
        {
            g_pClientEngfuncs->Con_Printf(
                "WARNING: DrawTriangles() hooking failed: entity "
                "report mode will not draw entity hull."
            );
        }
    }

    if (!g_hookIsThirdPerson.IsHooked() || !g_hookCameraOffset.IsHooked())
    {
        g_hookIsThirdPerson.Unhook();
        g_hookCameraOffset.Unhook();
        g_pClientEngfuncs->Con_Printf(
            "WARNING: IsThirdPerson() hooking failed: "
            "command gsm_thirdperson will not work."
        );
    }

    bool isHookSuccessful = g_hookRedraw.IsHooked() && g_hookPlayerMove.IsHooked();
    if (!isHookSuccessful)
    {
        RevertHooks();
        EXCEPT("unable to hook desired functions");
    }
}

void CHooks::Remove()
{
    // check for client.dll not already unloaded from process
    if (GetModuleHandle("client.dll"))
    {
        RevertHooks();
    }
}

void CHooks::RevertHooks()
{
    g_hookRedraw.Unhook();
    g_hookPlayerMove.Unhook();
    g_hookKeyEvent.Unhook();
    g_hookDrawTriangles.Unhook();
    g_hookIsThirdPerson.Unhook();
    g_hookCameraOffset.Unhook();
}
