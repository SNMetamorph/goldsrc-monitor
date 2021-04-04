#pragma once
#include "display_mode.h"

class CModeSpeedometer : public IDisplayMode
{
public: 
    CModeSpeedometer() {};
    virtual ~CModeSpeedometer() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };
    void HandleChangelevel() override {};

private:
    void DrawVelocityBar(int scrWidth, int scrHeight, float velocity);
};
