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
#include "string_stack.h"

enum class DisplayModeType
{
    Full,
    Speedometer,
    EntityReport,
    Measurement,
    FaceReport,
    AngleTracking
};

class IDisplayMode 
{
public:
    virtual ~IDisplayMode() {};
    virtual void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) = 0;
    virtual void Render3D() = 0;
    virtual bool KeyInput(bool keyDown, int keyCode, const char *bindName) = 0;
    virtual void HandleChangelevel() = 0;
    virtual DisplayModeType GetModeIndex() = 0;
};
