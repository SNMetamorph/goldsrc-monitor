#pragma once
#include "display_mode.h"
#include "hlsdk.h"

class CModeSpeedometer : public IDisplayMode
{
public: 
    CModeSpeedometer() {};
    virtual ~CModeSpeedometer() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(bool, int, const char *) override { return true; };
    void HandleChangelevel() override {};
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_SPEEDOMETER; };

private:
    void CalculateVelocity(float frameTime);
    void DrawVelocityBar(int scrWidth, int scrHeight, float velocity);

    float m_flVelocity;
    float m_flLastUpdateTime;
};
