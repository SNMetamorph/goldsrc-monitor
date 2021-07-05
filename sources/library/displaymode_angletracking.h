#pragma once
#include "display_mode.h"

class CModeAngleTracking : public IDisplayMode
{
public: 
    CModeAngleTracking() {};
    virtual ~CModeAngleTracking() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };
    void HandleChangelevel() override {}; 
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_ANGLETRACKING; };
};
