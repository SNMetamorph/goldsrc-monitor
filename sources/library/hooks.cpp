#include "stdafx.h"
#include "core.h"
#include "globals.h"
#include "exception.h"

// polyhook headers
#include "headers/CapstoneDisassembler.hpp"
#include "headers/Detour/x86Detour.hpp"

// hooking stuff
char		*funcRedraw;
char		*funcPlayerMove;
uint64_t	trpRedraw;
uint64_t	trpPlayerMove;
int			hookRedraw(float time, int intermission);
void		hookPlayerMove(playermove_t *pmove, int server);
typedef int (*pfnRedraw)(float, int);
typedef void (*pfnPlayerMove)(playermove_t*, int);

void apply_hooks()
{
	// find function addresses
	funcRedraw		= (char *)GetProcAddress(g_hClientModule, "HUD_Redraw");
	funcPlayerMove	= (char *)GetProcAddress(g_hClientModule, "HUD_PlayerMove");

	// applying hooks
	PLH::CapstoneDisassembler disasm(PLH::Mode::x86);
	PLH::x86Detour dtrRedraw(funcRedraw, (char *)&hookRedraw, &trpRedraw, disasm);
	PLH::x86Detour dtrPlayerMove(funcPlayerMove, (char *)&hookPlayerMove, &trpPlayerMove, disasm);

	if (!dtrRedraw.hook() || !dtrPlayerMove.hook())
		EXCEPT("unable to hook exported functions");
}

NOINLINE int __cdecl hookRedraw(float time, int intermission)
{
	PLH::FnCast(trpRedraw, pfnRedraw())(time, intermission);
	SCREENINFO scr_info;
	scr_info.iSize = sizeof(scr_info);
	g_pClientEngFuncs->pfnGetScreenInfo(&scr_info);
	frame_draw(time, intermission != 0, scr_info.iWidth, scr_info.iHeight);
	return 1;
}

NOINLINE void __cdecl hookPlayerMove(playermove_t *pmove, int server)
{
	PLH::FnCast(trpPlayerMove, pfnPlayerMove())(pmove, server);
	g_pPlayerMove = pmove;
}
