#include "stdafx.h"
#include "core.h"
#include "globals.h"
#include "exception.h"

// polyhook headers
#include "headers/CapstoneDisassembler.hpp"
#include "headers/Detour/x86Detour.hpp"

// hooking stuff
typedef int(*pfnRedraw)(float, int);
typedef void(*pfnPlayerMove)(playermove_t*, int);
typedef PLH::x86Detour detour_t;
typedef PLH::CapstoneDisassembler disassembler_t;

char		*funcRedraw;
char		*funcPlayerMove;
uint64_t	trpRedraw;
uint64_t	trpPlayerMove;
int			hookRedraw(float time, int intermission);
void		hookPlayerMove(playermove_t *pmove, int server);

detour_t		*dtrRedraw;
detour_t		*dtrPlayerMove;
disassembler_t	*addrDisasm;

void ApplyHooks()
{
	// find function addresses
	funcRedraw		= (char *)GetProcAddress(g_hClientModule, "HUD_Redraw");
	funcPlayerMove	= (char *)GetProcAddress(g_hClientModule, "HUD_PlayerMove");

	// initializing & applying hooks
	addrDisasm		= new disassembler_t(PLH::Mode::x86);
	dtrRedraw		= new detour_t(funcRedraw, (char *)&hookRedraw, &trpRedraw, *addrDisasm);
	dtrPlayerMove	= new detour_t(funcPlayerMove, (char *)&hookPlayerMove, &trpPlayerMove, *addrDisasm);
	
	if (!dtrRedraw || !dtrPlayerMove || !addrDisasm)
		EXCEPT("failed to allocate hooking stuff");

	if (!dtrRedraw->hook() || !dtrPlayerMove->hook())
	{
		delete dtrRedraw;
		delete dtrPlayerMove;
		delete addrDisasm;
		EXCEPT("unable to hook desired functions");
	}
}

void RemoveHooks()
{
	if (!dtrRedraw || !dtrPlayerMove || !GetModuleHandle("client.dll"))
		return;

	dtrRedraw->unHook();
	dtrPlayerMove->unHook();
	delete dtrRedraw;
	delete dtrPlayerMove;
	delete addrDisasm;
}

NOINLINE int __cdecl hookRedraw(float time, int intermission)
{
	// call original function
	PLH::FnCast(trpRedraw, pfnRedraw())(time, intermission);

	SCREENINFO scr_info;
	scr_info.iSize = sizeof(scr_info);
	g_pClientEngFuncs->pfnGetScreenInfo(&scr_info);

	FrameDraw(time, intermission != 0, scr_info.iWidth, scr_info.iHeight);
	return 1;
}

NOINLINE void __cdecl hookPlayerMove(playermove_t *pmove, int server)
{
	PLH::FnCast(trpPlayerMove, pfnPlayerMove())(pmove, server);
	g_pPlayerMove = pmove;
}
