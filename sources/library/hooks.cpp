#include "stdafx.h"
#include "hooks.h"
#include "core.h"
#include "globals.h"
#include "exception.h"

// polyhook headers
#include "headers/CapstoneDisassembler.hpp"
#include "headers/Detour/x86Detour.hpp"

// hooking stuff
typedef int (*pfnRedraw)(float, int);
typedef void (*pfnPlayerMove)(playermove_t*, int);
typedef int (*pfnKeyEvent)(int, int, const char *);
typedef void (*pfnDrawTriangles)();

typedef PLH::x86Detour detour_t;
typedef PLH::CapstoneDisassembler disasm_t;

static uint64_t     g_pfnOrigRedraw;
static uint64_t     g_pfnOrigPlayerMove;
static uint64_t     g_pfnOrigKeyEvent;
static uint64_t     g_pfnOrigDrawTriangles;

static detour_t	    *g_DetourRedraw;
static detour_t     *g_DetourPlayerMove;
static detour_t     *g_DetourKeyEvent;
static detour_t     *g_DetourDrawNormalTriangles;
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
    if (g_DetourDrawNormalTriangles)
        g_DetourDrawNormalTriangles->unHook();

    delete g_DetourRedraw;
    delete g_DetourKeyEvent;
    delete g_DetourPlayerMove;
    delete g_DetourDrawNormalTriangles;
}

void ApplyHooks()
{
    char *pfnRedraw = (char *)GetProcAddress(g_hClientModule, "HUD_Redraw");
    char *pfnPlayerMove = (char *)GetProcAddress(g_hClientModule, "HUD_PlayerMove");
    char *pfnKeyEvent = (char *)GetProcAddress(g_hClientModule, "HUD_Key_Event");
    char *pfnDrawTriangles = (char *)GetProcAddress(g_hClientModule, "HUD_DrawTransparentTriangles");

    char *pfnHookRedraw     = (char *)&HookRedraw;
    char *pfnHookPlayerMove = (char *)&HookPlayerMove;
    char *pfnHookKeyEvent   = (char *)&HookKeyEvent;
    char *pfnHookDrawNormalTriangles = (char *)&HookDrawTriangles;

    g_DetourRedraw = new detour_t(pfnRedraw, pfnHookRedraw, &g_pfnOrigRedraw, g_DisasmX86);
    g_DetourKeyEvent = new detour_t(pfnKeyEvent, pfnHookKeyEvent, &g_pfnOrigKeyEvent, g_DisasmX86);
    g_DetourPlayerMove = new detour_t(
        pfnPlayerMove, pfnHookPlayerMove, &g_pfnOrigPlayerMove, g_DisasmX86
    );
    g_DetourDrawNormalTriangles = new detour_t(
        pfnDrawTriangles, pfnHookDrawNormalTriangles, &g_pfnOrigDrawTriangles, g_DisasmX86
    );

    if (!g_DetourRedraw || !g_DetourPlayerMove || !g_DetourKeyEvent || !g_DetourDrawNormalTriangles)
    {
        DisposeHookingStuff();
        EXCEPT("failed to allocate hooking stuff");
    }

    bool isHookSuccessful = (
        g_DetourRedraw->hook() &&
        g_DetourPlayerMove->hook() &&
        g_DetourKeyEvent->hook() &&
        g_DetourDrawNormalTriangles->hook()
    );

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
    AssignDisplayMode();

    if (g_pPlayerMove)
    {
        bool isIntermission = intermission != 0;
        g_ScreenInfo.iSize = sizeof(g_ScreenInfo);
        g_pClientEngFuncs->pfnGetScreenInfo(&g_ScreenInfo);
        g_pDisplayMode->Render2D(g_ScreenInfo.iWidth, g_ScreenInfo.iHeight);
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
    return returnCode && g_pDisplayMode->KeyInput(keyDown, keyCode, bindName);
}

NOINLINE static void __cdecl HookDrawTriangles()
{
    PLH::FnCast(g_pfnOrigDrawTriangles, pfnDrawTriangles())();
    g_pDisplayMode->Render3D();
}