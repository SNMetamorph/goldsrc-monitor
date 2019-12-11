#pragma once
#include "globals.h"
#include "module_info.h"

enum display_mode_t
{
	DISPLAYMODE_FULL,
	DISPLAYMODE_SPEEDOMETER,
};

void	PrintTitleText();
int		GetStringWidth(const char *str);
void	SetupCvars(module_info_t &engine_lib);
void	FrameDraw(float time, bool intermission, int scr_width, int scr_height);
