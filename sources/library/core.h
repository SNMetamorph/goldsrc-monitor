#pragma once
#include "globals.h"
#include "module_info.h"

enum
{
	MODE_FULL,
	MODE_SPEEDOMETER,
};

void	show_intro_message();
int		get_string_width(const char *str);
void	init_stuff(module_info_t &engine_lib);
void	frame_draw(float time, bool intermission, int scr_width, int scr_height);
