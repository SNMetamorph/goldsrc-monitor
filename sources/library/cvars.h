/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#pragma once
#include "hlsdk.h"

class ConVars
{
public:
    static cvar_t *gsm_color_r;
    static cvar_t *gsm_color_g;
    static cvar_t *gsm_color_b;
    static cvar_t *gsm_margin_up;
    static cvar_t *gsm_margin_right;
    static cvar_t *gsm_mode;
    static cvar_t *gsm_debug;
    static cvar_t *gsm_thirdperson;
    static cvar_t *gsm_thirdperson_dist;
    static cvar_t *sys_timescale;
};
