#pragma once
#include "display_mode.h"

class CModeFull : public IDisplayMode
{
public: 
    static CModeFull &GetInstance();

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };

private:
    CModeFull() {};
    CModeFull(const CModeFull&) = delete;
    CModeFull& operator=(const CModeFull&) = delete;

    float GetSmoothFrameTime();
    const char *GetMovetypeName(int moveType);
};
