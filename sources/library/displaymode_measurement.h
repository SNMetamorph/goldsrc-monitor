#pragma once
#include "stdafx.h"
#include "display_mode.h"

class CModeMeasurement : public IDisplayMode
{
public:
    enum
    {
        SNAPMODE_FREE,
        SNAPMODE_AXIS_X,
        SNAPMODE_AXIS_Y,
        SNAPMODE_AXIS_Z,
        SNAPMODE_ALONGLINE,
        SNAPMODE_MAX,
    };

public:
    static CModeMeasurement &GetInstance();

    void Render2D(int screenWidth, int screenHeight) override;
    void Render3D() override {};
    bool KeyInput(int isKeyDown, int keyCode, const char *) override;

    const vec3_t&   GetPointOriginA();
    const vec3_t&   GetPointOriginB();
    const char*     GetSnapModeName();
    float           GetPointsDistance();
    float           GetLineElevationAngle();

private:
    CModeMeasurement() {};
    CModeMeasurement(const CModeMeasurement&) = delete;
    CModeMeasurement& operator=(const CModeMeasurement&) = delete;

    void UpdatePointOrigin(vec3_t &linePoint, const vec3_t &targetPoint);
    void TraceAlongNormal(pmtrace_t &traceData, float traceLength);
    bool WorldToScreen(int w, int h, int &x, int &y, vec3_t &origin);
    void DrawVisualization(int screenWidth, int screenHeight);
    void DrawMeasurementLine(float lifeTime);
    void DrawPointHints(int screenWidth, int screenHeight);
    void DrawSupportLines(float lifeTime);
    void LoadLineSprite();

    vec3_t       m_vecPointA;
    vec3_t       m_vecPointB;
    HLSPRITE     m_iLineSprite;
    int          m_iSnapMode = SNAPMODE_FREE;
};

extern CModeMeasurement &g_ModeMeasurement;
