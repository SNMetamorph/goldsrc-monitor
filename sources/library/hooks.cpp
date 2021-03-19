#include "stdafx.h"
#include "hooks.h"
#include "application.h"
#include "client_module.h"
#include "exception.h"

// polyhook headers
#include <polyhook2/ZydisDisassembler.hpp>
#include <polyhook2/Detour/x86Detour.hpp>

// hooking stuff
typedef int (*pfnRedraw)(float, int);
typedef void (*pfnPlayerMove)(playermove_t*, int);
typedef int (*pfnKeyEvent)(int, int, const char *);
typedef void (*pfnDrawTriangles)();

typedef PLH::x86Detour detour_t;
typedef PLH::ZydisDisassembler disasm_t;

static uint64_t     g_pfnOrigRedraw;
static uint64_t     g_pfnOrigPlayerMove;
static uint64_t     g_pfnOrigKeyEvent;
static uint64_t     g_pfnOrigDrawTriangles;

static detour_t	    *g_DetourRedraw;
static detour_t     *g_DetourPlayerMove;
static detour_t     *g_DetourKeyEvent;
static detour_t     *g_DetourDrawTriangles;
static disasm_t     g_DisasmX86(PLH::Mode::x86);

static int  HookRedraw(float time, int intermission);
static int  HookKeyEvent(int down, int keyNum, const char *bindName);
static void HookPlayerMove(playermove_t *pmove, int server);
static void HookDrawTriangles();

static void DisposeHookingStuff()
{
    if (g_DetourRedraw)
        g_DetourRedraw->unHook();
    if (g_DetourKeyEvent)
        g_DetourKeyEvent->unHook();
    if (g_DetourPlayerMove)
        g_DetourPlayerMove->unHook();
    if (g_DetourDrawTriangles)
        g_DetourDrawTriangles->unHook();

    delete g_DetourRedraw;
    delete g_DetourKeyEvent;
    delete g_DetourPlayerMove;
    delete g_DetourDrawTriangles;
}

void ApplyHooks()
{
    char *pfnRedraw = (char *)GetProcAddress(g_ClientModule.GetHandle(), "HUD_Redraw");
    char *pfnPlayerMove = (char *)GetProcAddress(g_ClientModule.GetHandle(), "HUD_PlayerMove");
    char *pfnKeyEvent = (char *)GetProcAddress(g_ClientModule.GetHandle(), "HUD_Key_Event");
    char *pfnDrawTriangles = (char *)GetProcAddress(g_ClientModule.GetHandle(), "HUD_DrawTransparentTriangles");

    char *pfnHookRedraw     = (char *)&HookRedraw;
    char *pfnHookPlayerMove = (char *)&HookPlayerMove;
    char *pfnHookKeyEvent   = (char *)&HookKeyEvent;
    char *pfnHookDrawTriangles = (char *)&HookDrawTriangles;

    g_DetourRedraw = new detour_t(pfnRedraw, pfnHookRedraw, &g_pfnOrigRedraw, g_DisasmX86);
    g_DetourKeyEvent = new detour_t(pfnKeyEvent, pfnHookKeyEvent, &g_pfnOrigKeyEvent, g_DisasmX86);
    g_DetourPlayerMove = new detour_t(
        pfnPlayerMove, pfnHookPlayerMove, &g_pfnOrigPlayerMove, g_DisasmX86
    );
    g_DetourDrawTriangles = new detour_t(
        pfnDrawTriangles, pfnHookDrawTriangles, &g_pfnOrigDrawTriangles, g_DisasmX86
    );

    if (!g_DetourRedraw || 
        !g_DetourPlayerMove || 
        !g_DetourKeyEvent || 
        !g_DetourDrawTriangles)
    {
        DisposeHookingStuff();
        EXCEPT("failed to allocate hooking stuff");
    }

    bool isHookSuccessful = (
        g_DetourRedraw->hook() &&
        g_DetourPlayerMove->hook()
    );

    if (!g_DetourKeyEvent->hook())
    {
        g_pClientEngfuncs->Con_Printf(
            "WARNING: KeyEvent() hooking failed: "
            "measurement mode will not react to keys."
        );
    }

    if (!g_DetourDrawTriangles->hook())
    {
        pfnDrawTriangles = (char *)GetProcAddress(g_ClientModule.GetHandle(), "HUD_DrawNormalTriangles");

        delete g_DetourDrawTriangles;
        g_DetourDrawTriangles = new detour_t(
            pfnDrawTriangles, pfnHookDrawTriangles, &g_pfnOrigDrawTriangles, g_DisasmX86
        );

        if (!g_DetourDrawTriangles->hook())
        {
            g_pClientEngfuncs->Con_Printf(
                "WARNING: DrawTriangles() hooking failed: entity "
                "report mode will not draw entity hull lines."
            );
        }
    }

    if (!isHookSuccessful)
    {
        DisposeHookingStuff();
        EXCEPT("unable to hook desired functions");
    }
}

void RemoveHooks()
{
    // check for client.dll not already unloaded from process
    if (GetModuleHandle("client.dll"))
        DisposeHookingStuff();
}

NOINLINE static int __cdecl HookRedraw(float time, int intermission)
{
    // call original function
    PLH::FnCast(g_pfnOrigRedraw, pfnRedraw())(time, intermission);
    if (g_pPlayerMove)
    {
        bool isIntermission = intermission != 0;
        g_Application.DisplayModeRender2D();
    }
    return 1;
}

NOINLINE static void __cdecl HookPlayerMove(playermove_t *pmove, int server)
{
    PLH::FnCast(g_pfnOrigPlayerMove, pfnPlayerMove())(pmove, server);
    g_pPlayerMove = pmove;
}

NOINLINE static int __cdecl HookKeyEvent(int keyDown, int keyCode, const char *bindName)
{
    int returnCode = PLH::FnCast(g_pfnOrigKeyEvent, pfnKeyEvent())(
        keyDown, keyCode, bindName
    );
    return returnCode && g_Application.KeyInput(keyDown, keyCode, bindName);
}

NOINLINE static void __cdecl HookDrawTriangles()
{
    PLH::FnCast(g_pfnOrigDrawTriangles, pfnDrawTriangles())();
    g_Application.DisplayModeRender3D();
}