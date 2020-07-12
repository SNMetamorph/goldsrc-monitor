#pragma once
#include "stdafx.h"
#include "display_mode.h"

class CModeEntityReport : public IDisplayMode
{
public: 
    static CModeEntityReport &GetInstance();

    void Render2D(int scrWidth, int scrHeight) override;
    void Render3D() override;
    bool KeyInput(int, int, const char *) override { return true; };

private:
    CModeEntityReport() {};
    CModeEntityReport(const CModeEntityReport&) = delete;
    CModeEntityReport& operator=(const CModeEntityReport&) = delete;

    bool TraceVisEnt(vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);

    int m_iEntityIndex;
    vec3_t m_vecBboxMin;
    vec3_t m_vecBboxMax;
};

extern CModeEntityReport &g_ModeEntityReport;
