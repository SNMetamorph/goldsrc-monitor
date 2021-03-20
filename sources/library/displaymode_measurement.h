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
    CModeMeasurement() {};
    virtual ~CModeMeasurement() {};

    void Render2D(int screenWidth, int screenHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(int isKeyDown, int keyCode, const char *) override;
    void HandleChangelevel() override;

private:
    const vec3_t &GetPointOriginA() const;
    const vec3_t &GetPointOriginB() const;
    const char *GetSnapModeName() const;
    float GetPointsDistance() const;
    float GetLineElevationAngle() const;

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
