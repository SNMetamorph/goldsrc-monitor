#pragma once
#include "display_mode.h"

class CModeFull : public IDisplayMode
{
public: 
    CModeFull() {};
    virtual ~CModeFull() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(int, int, const char *) override { return true; };
    void HandleChangelevel() override {};

private:
    float GetSmoothFrameTime();
    const char *GetMovetypeName(int moveType);
};
