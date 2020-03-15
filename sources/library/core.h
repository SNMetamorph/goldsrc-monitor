#pragma once
#include "globals.h"
#include "moduleinfo.h"

enum display_mode_t
{
	DISPLAYMODE_FULL,
	DISPLAYMODE_SPEEDOMETER,
    DISPLAYMODE_ENTITYREPORT,
    DISPLAYMODE_ANGLETRACKING,
};

void	PrintTitleText();
int		GetStringWidth(const char *str);
void	SetupConVars(moduleinfo_t &engine_lib);
void	FrameDraw(float time, bool intermission, int scr_width, int scr_height);
