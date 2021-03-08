#pragma once
#include "display_mode.h"

class CModeAngleTracking : public IDisplayMode
{
public: 
    static CModeAngleTracking &GetInstance();

    void Render2D(int scrWidth, int scrHeight) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };

private:
    CModeAngleTracking() {};
    CModeAngleTracking(const CModeAngleTracking&) = delete;
    CModeAngleTracking& operator=(const CModeAngleTracking&) = delete;
};
