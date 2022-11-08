#pragma once
#include "display_mode.h"

class CModeFull : public IDisplayMode
{
public: 
    CModeFull();
    virtual ~CModeFull() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(bool, int, const char *) override { return true; };
    void HandleChangelevel() override {};
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_FULL; };

private:
    float GetSmoothSystemFrametime();

    float m_flFrameTime;
    float m_flLastFrameTime;
    float m_flLastSysTime;
};
