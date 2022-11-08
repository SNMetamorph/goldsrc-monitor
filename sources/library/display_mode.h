#pragma once
#include "string_stack.h"

enum DisplayModeIndex
{
    DISPLAYMODE_FULL,
    DISPLAYMODE_SPEEDOMETER,
    DISPLAYMODE_ENTITYREPORT,
    DISPLAYMODE_MEASUREMENT,
    DISPLAYMODE_FACEREPORT,
    DISPLAYMODE_ANGLETRACKING
};

class IDisplayMode 
{
public:
    virtual ~IDisplayMode() {};
    virtual void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) = 0;
    virtual void Render3D() = 0;
    virtual bool KeyInput(bool keyDown, int keyCode, const char *bindName) = 0;
    virtual void HandleChangelevel() = 0;
    virtual DisplayModeIndex GetModeIndex() = 0;
};
