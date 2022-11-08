#pragma once
#include "display_mode.h"
#include "hlsdk.h"

class CModeAngleTracking : public IDisplayMode
{
public: 
    CModeAngleTracking();
    virtual ~CModeAngleTracking() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(bool, int, const char *) override { return true; };
    void HandleChangelevel() override {}; 
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_ANGLETRACKING; };

private:
    vec3_t m_vecLastAngles;
    float m_flTrackStartTime;
    float m_flLastYawVelocity;
    float m_flLastPitchVelocity;
};
