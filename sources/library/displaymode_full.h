#pragma once
#include "display_mode.h"

class CModeFull : public IDisplayMode
{
public: 
    static CModeFull &GetInstance();

    void Render2D(int scrWidth, int scrHeight) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };

private:
    CModeFull() {};
    CModeFull(const CModeFull&) = delete;
    CModeFull& operator=(const CModeFull&) = delete;

    float GetSmoothFrameTime();
};

extern CModeFull &g_ModeFull;
