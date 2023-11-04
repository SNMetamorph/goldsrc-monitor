/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#pragma once
#include "stdafx.h"
#include "display_mode.h"
#include "local_player.h"

class CModeMeasurement : public IDisplayMode
{
public:
    enum class SnapMode
    {
        Free,
        AxisX,
        AxisY,
        AxisZ,
        AlongLine,
        Count,
    };

public:
    CModeMeasurement(const CLocalPlayer &playerRef);
    virtual ~CModeMeasurement() {};

    void Render2D(float frameTime, int screenWidth, int screenHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(bool keyDown, int keyCode, const char *) override;
    void HandleChangelevel() override;
    DisplayModeType GetModeIndex() override { return DisplayModeType::Measurement; };

private:
    const vec3_t &GetPointOriginA() const;
    const vec3_t &GetPointOriginB() const;
    const char *GetSnapModeName() const;
    float GetPointsDistance() const;
    float GetLineElevationAngle() const;

    void UpdatePointOrigin(vec3_t &linePoint, const vec3_t &targetPoint);
    void TraceAlongNormal(pmtrace_t &traceData, float traceLength);
    void DrawVisualization(float frameTime, int screenWidth, int screenHeight);
    void DrawMeasurementLine(float lifeTime);
    void PrintPointHints(int screenWidth, int screenHeight);
    void PrintLineLength(int screenWidth, int screenHeight, vec3_t pointStart, vec3_t pointEnd);
    void DrawSupportLines(float lifeTime);
    void DrawLineProjections(int screenWidth, int screenHeight, float lifeTime);
    void LoadLineSprite();

    vec3_t       m_pointA;
    vec3_t       m_pointB;
    HLSPRITE     m_lineSprite;
    SnapMode     m_snapMode;
    const CLocalPlayer &m_localPlayer;
};
