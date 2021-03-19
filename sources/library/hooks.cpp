#include "stdafx.h"
#include "hooks.h"
#include "application.h"
#include "client_module.h"
#include "exception.h"
#include "function_hook.h"

// hooking stuff
typedef int (*pfnRedraw_t)(float, int);
typedef void (*pfnPlayerMove_t)(playermove_t*, int);
typedef int (*pfnKeyEvent_t)(int, int, const char *);
typedef void (*pfnDrawTriangles_t)();

CFunctionHook<pfnRedraw_t> hookRedraw;
CFunctionHook<pfnPlayerMove_t> hookPlayerMove;
CFunctionHook<pfnKeyEvent_t> hookKeyEvent;
CFunctionHook<pfnDrawTriangles_t> hookDrawTriangles;

static int  HookRedraw(float time, int intermission);
static int  HookKeyEvent(int down, int keyNum, const char *bindName);
static void HookPlayerMove(playermove_t *pmove, int server);
static void HookDrawTriangles();

static void DisposeHooks()
{
    if (hookRedraw.IsHooked())
        hookRedraw.Unhook();
    if (hookPlayerMove.IsHooked())
        hookPlayerMove.Unhook();
    if (hookKeyEvent.IsHooked())
        hookKeyEvent.Unhook();
    if (hookDrawTriangles.IsHooked())
        hookDrawTriangles.Unhook();
}

void Hooks::Apply()
{
    pfnRedraw_t pfnRedraw = (pfnRedraw_t)g_ClientModule.GetFuncAddress("HUD_Redraw");
    pfnPlayerMove_t pfnPlayerMove = (pfnPlayerMove_t)g_ClientModule.GetFuncAddress("HUD_PlayerMove");
    pfnKeyEvent_t pfnKeyEvent = (pfnKeyEvent_t)g_ClientModule.GetFuncAddress("HUD_Key_Event");
    pfnDrawTriangles_t pfnDrawTriangles = (pfnDrawTriangles_t)g_ClientModule.GetFuncAddress("HUD_DrawTransparentTriangles");

    hookRedraw.Hook(pfnRedraw, &HookRedraw);
    hookPlayerMove.Hook(pfnPlayerMove, &HookPlayerMove);
    hookKeyEvent.Hook(pfnKeyEvent, &HookKeyEvent);
    hookDrawTriangles.Hook(pfnDrawTriangles, &HookDrawTriangles);

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

    bool isHookSuccessful = hookRedraw.IsHooked() && hookPlayerMove.IsHooked();
    if (!isHookSuccessful)
    {
        DisposeHooks();
        EXCEPT("unable to hook desired functions");
    }
}

void Hooks::Remove()
{
    // check for client.dll not already unloaded from process
    if (GetModuleHandle("client.dll"))
    {
        DisposeHooks();
    }
}

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
    g_pPlayerMove = pmove;
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
