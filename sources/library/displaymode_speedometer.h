#pragma once
#include "display_mode.h"

class CModeSpeedometer : public IDisplayMode
{
public: 
    static CModeSpeedometer &GetInstance();

    void Render2D(int scrWidth, int scrHeight) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };

private:
    CModeSpeedometer() {};
    CModeSpeedometer(const CModeSpeedometer&) = delete;
    CModeSpeedometer& operator=(const CModeSpeedometer&) = delete;
};

extern CModeSpeedometer &g_ModeSpeedometer;
