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
typedef PLH::x86Detour detour_t;
typedef PLH::CapstoneDisassembler disasm_t;

static uint64_t     g_pfnOrigRedraw;
static uint64_t     g_pfnOrigPlayerMove;
static detour_t	    *g_DetourRedraw;
static detour_t     *g_DetourPlayerMove;
static disasm_t     *g_pDisasm;

static int  HookRedraw(float time, int intermission);
static void HookPlayerMove(playermove_t *pmove, int server);

void ApplyHooks()
{
    // find function addresses
    char *pfnRedraw = (char *)GetProcAddress(g_hClientModule, "HUD_Redraw");
    char *pfnPlayerMove = (char *)GetProcAddress(g_hClientModule, "HUD_PlayerMove");
    char *pfnHookRedraw = (char *)&HookRedraw;
    char *pfnHookPlayerMove = (char *)&HookPlayerMove;
    uint64_t *pfnOrigRedraw = (uint64_t *)&g_pfnOrigRedraw;
    uint64_t *pfnOrigPlayerMove = (uint64_t *)&g_pfnOrigPlayerMove;

    // initializing & applying hooks
    g_pDisasm = new disasm_t(PLH::Mode::x86);
    g_DetourRedraw = new detour_t(pfnRedraw, pfnHookRedraw, pfnOrigRedraw, *g_pDisasm);
    g_DetourPlayerMove = new detour_t(
        pfnPlayerMove, pfnHookPlayerMove, pfnOrigPlayerMove, *g_pDisasm
    );

    if (!g_DetourRedraw || !g_DetourPlayerMove || !g_pDisasm)
        EXCEPT("failed to allocate hooking stuff");

    if (!g_DetourRedraw->hook() || !g_DetourPlayerMove->hook())
    {
        delete g_DetourRedraw;
        delete g_DetourPlayerMove;
        delete g_pDisasm;
        EXCEPT("unable to hook desired functions");
    }
}

void RemoveHooks()
{
    if (!g_DetourRedraw || !g_DetourPlayerMove || !GetModuleHandle("client.dll"))
        return;

    g_DetourRedraw->unHook();
    g_DetourPlayerMove->unHook();
    delete g_DetourRedraw;
    delete g_DetourPlayerMove;
    delete g_pDisasm;
}

NOINLINE int __cdecl HookRedraw(float time, int intermission)
{
    // call original function
    PLH::FnCast(g_pfnOrigRedraw, pfnRedraw())(time, intermission);

    if (g_pPlayerMove)
    {
        bool isIntermission = intermission != 0;
        g_ScreenInfo.iSize = sizeof(g_ScreenInfo);
        g_pClientEngFuncs->pfnGetScreenInfo(&g_ScreenInfo);
        FrameDraw(time, isIntermission, g_ScreenInfo.iWidth, g_ScreenInfo.iHeight);
    }

    return 1;
}

NOINLINE void __cdecl HookPlayerMove(playermove_t *pmove, int server)
{
    PLH::FnCast(g_pfnOrigPlayerMove, pfnPlayerMove())(pmove, server);
    g_pPlayerMove = pmove;
}
