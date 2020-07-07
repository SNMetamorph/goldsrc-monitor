#pragma once
#include "stdafx.h"

enum
{
    SNAPMODE_FREE,
    SNAPMODE_AXIS_X,
    SNAPMODE_AXIS_Y,
    SNAPMODE_AXIS_Z,
    SNAPMODE_MAX,
};

class CMeasurement
{
public:
    static CMeasurement &GetInstance();
    void HandleInput(int keyCode);
    void Visualize(int screenWidth, int screenHeight);

    const vec3_t&   GetPointOriginA();
    const vec3_t&   GetPointOriginB();
    const char*     GetSnapModeName();
    float           GetPointsDistance();
    float           GetLineElevationAngle();

private:
    void UpdatePointOrigin(vec3_t &destPoint, const vec3_t &srcPoint);
    void TraceAlongNormal(pmtrace_t &traceData, float traceLength);
    bool WorldToScreen(int w, int h, int &x, int &y, vec3_t &origin);
    void DrawMeasurementLine(float lifeTime);
    void DrawPointHints(int screenWidth, int screenHeight);
    void DrawSupportLines(float lifeTime);
    void LoadLineSprite();

    vec3_t       m_vecPointA;
    vec3_t       m_vecPointB;
    HLSPRITE     m_iLineSprite;
    int          m_iSnapMode = SNAPMODE_FREE;
};

extern CMeasurement &g_Measurement;
