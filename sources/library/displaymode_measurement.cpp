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

#include "displaymode_measurement.h"
#include "utils.h"
#include "client_module.h"
#include "cvars.h"
#include "local_player.h"

// HLSDK
#include "keydefs.h"

CModeMeasurement::CModeMeasurement()
{
    const vec3_t zeroVector = { 0.0f, 0.0f, 0.0f };
    m_pointA = zeroVector;
    m_pointB = zeroVector;
    m_lineSprite = 0;
    m_snapMode = SnapMode::Free;
}

void CModeMeasurement::UpdatePointOrigin(vec3_t &linePoint, const vec3_t &targetPoint)
{
    if (m_snapMode != SnapMode::AlongLine)
    {
        switch (m_snapMode)
        {
        case SnapMode::AxisX:
            linePoint.x = targetPoint.x;
            break;
        case SnapMode::AxisY:
            linePoint.y = targetPoint.y;
            break;
        case SnapMode::AxisZ:
            linePoint.z = targetPoint.z;
            break;
        default:
            linePoint = targetPoint;
            break;
        }
    }
    else
    {
        vec3_t lineVector = m_pointB - m_pointA;
        if (lineVector.Length() > 0.0f)
        {
            vec3_t targVector = targetPoint - linePoint;
            float projLen = DotProduct(lineVector, targVector) / lineVector.Length();
            linePoint = linePoint + lineVector.Normalize() * projLen;
        }
    }
}

void CModeMeasurement::TraceAlongNormal(pmtrace_t &traceData, float traceLength)
{
    vec3_t traceOrigin      = traceData.endpos;
    vec3_t planeNormal      = traceData.plane.normal;
    vec3_t *pointsList[2]   = { &m_pointA, &m_pointB };

    for (int i = 0; i < 2; ++i)
    {
        float directionSign = -1.0f + 2.0f * (i % 2);
        vec3_t traceDir = planeNormal * directionSign;
        Utils::TraceLine(traceOrigin, traceDir, traceLength, &traceData);
        *pointsList[i] = traceData.endpos;
    }
}

void CModeMeasurement::DrawVisualization(float frameTime, int screenWidth, int screenHeight)
{
    float lifeTime = std::min(0.05f, frameTime);
    DrawMeasurementLine(lifeTime);
    DrawLineProjections(screenWidth, screenHeight, lifeTime);
    PrintPointHints(screenWidth, screenHeight);
    PrintLineLength(screenWidth, screenHeight, m_pointA, m_pointB);

    if (m_snapMode != SnapMode::Free && m_snapMode != SnapMode::AlongLine) {
        DrawSupportLines(lifeTime);
    }
}

const vec3_t& CModeMeasurement::GetPointOriginA() const
{
    return m_pointA;
}

const vec3_t& CModeMeasurement::GetPointOriginB() const
{
    return m_pointB;
}

float CModeMeasurement::GetPointsDistance() const
{
    return (m_pointB - m_pointA).Length();
}

bool CModeMeasurement::KeyInput(bool keyDown, int keyCode, const char *)
{
    vec3_t viewDir;
    vec3_t viewOrigin;
    vec3_t viewAngles;
    pmtrace_t traceData;
    const float traceLen = 64000.f;

    if (Utils::GetCurrentDisplayMode() != DisplayModeType::Measurement || !keyDown)
        return true;

    if (keyCode >= K_MOUSE1 && keyCode <= K_MOUSE3)
    {
        g_pClientEngfuncs->pfnPlaySoundByName("buttons/lightswitch2.wav", 1.0f);
        g_pClientEngfuncs->GetViewAngles(viewAngles);
        g_pClientEngfuncs->pfnAngleVectors(viewAngles, viewDir, nullptr, nullptr);
        viewOrigin = g_LocalPlayer.GetViewOrigin();
        Utils::TraceLine(viewOrigin, viewDir, traceLen, &traceData);

        if (keyCode == K_MOUSE1)
            UpdatePointOrigin(m_pointA, traceData.endpos);
        else if (keyCode == K_MOUSE2)
            UpdatePointOrigin(m_pointB, traceData.endpos);
        else if (keyCode == K_MOUSE3)
            TraceAlongNormal(traceData, traceLen);
        return false;
    }
    else if (keyCode == 'v')
    {
        m_snapMode = static_cast<SnapMode>(static_cast<uint32_t>(m_snapMode) + 1);
        if (m_snapMode == SnapMode::Count) {
            m_snapMode = SnapMode::Free;
        }
        g_pClientEngfuncs->pfnPlaySoundByName("buttons/blip1.wav", 0.8f);
        return false;
    }
    return true;
}

void CModeMeasurement::HandleChangelevel()
{
    const vec3_t vecNull = vec3_t(0, 0, 0);
    m_pointA = vecNull;
    m_pointB = vecNull;
    m_snapMode = SnapMode::Free;
    m_lineSprite = 0;
}

void CModeMeasurement::DrawMeasurementLine(float lifeTime)
{
    const float lineWidth       = 0.7f;
    const float lineBrightness  = 1.0f;
    const float lineSpeed       = 5.0f;
    const float lineColorR      = 0.1f;
    const float lineColorG      = 1.0f;
    const float lineColorB      = 0.11f;

    g_pClientEngfuncs->pEfxAPI->R_BeamPoints(
        m_pointA, m_pointB, m_lineSprite,
        lifeTime * 2.f, lineWidth, 0,
        lineBrightness, lineSpeed, 0, 0,
        lineColorR, lineColorG, lineColorB
    );
}

void CModeMeasurement::PrintPointHints(int screenWidth, int screenHeight)
{
    const int textColorR = 39;
    const int textColorG = 227;
    const int textColorB = 198;

    Utils::DrawString3D(m_pointA, "A", textColorR, textColorG, textColorB);
    Utils::DrawString3D(m_pointB, "B", textColorR, textColorG, textColorB);
}

void CModeMeasurement::PrintLineLength(int screenWidth, int screenHeight, vec3_t pointStart, vec3_t pointEnd)
{
    std::string lengthStr;
    vec3_t lineMiddle = (pointStart + pointEnd) / 2.f;
    const int textColorR = 255;
    const int textColorG = 255;
    const int textColorB = 20;

    float distance = (pointStart - pointEnd).Length();
    if (distance > 0.0f)
    {
        vec3_t offset = vec3_t(0.0f, 0.0f, 1.0f) * std::powf(distance, 0.5f);
        Utils::Snprintf(lengthStr, "%.2f", distance);
        Utils::DrawString3D(lineMiddle + offset, lengthStr.c_str(), textColorR, textColorG, textColorB);
    }
}

void CModeMeasurement::DrawSupportLines(float lifeTime)
{
    vec3_t axisVector               = { 0.f, 0.f, 0.f };
    const vec3_t *pointsList[2]     = { &m_pointA, &m_pointB };
    const float lineWidth           = 0.7f;
    const float lineLenght          = 24.0f;
    const float lineSpeed           = 5.0f;
    const float lineColorR          = 1.0f;
    const float lineColorG          = 0.0f;
    const float lineColorB          = 0.5f;
    const float lineBrightness      = 1.2f;

    if (m_snapMode == SnapMode::AxisX)
        axisVector.x = 1.f;
    else if (m_snapMode == SnapMode::AxisY)
        axisVector.y = 1.f;
    else if (m_snapMode == SnapMode::AxisZ)
        axisVector.z = 1.f;

    for (int i = 0; i < 2; ++i)
    {
        g_pClientEngfuncs->pEfxAPI->R_BeamPoints(
            *pointsList[i] + (axisVector * lineLenght),
            *pointsList[i] - (axisVector * lineLenght),
            m_lineSprite,
            lifeTime * 2.0f, lineWidth, 0,
            lineBrightness, lineSpeed, 0, 0,
            lineColorR, lineColorG, lineColorB
        );
    }
}

void CModeMeasurement::DrawLineProjections(int screenWidth, int screenHeight, float lifeTime)
{
    bool baseFound = false;
    vec3_t basePoint = m_pointA;
    const float lineWidth = 0.7f;
    const float lineLenght = 24.0f;
    const float lineSpeed = 5.0f;
    const float lineColorR = 1.0f;
    const float lineColorG = 0.08f;
    const float lineColorB = 0.08f;
    const float lineBrightness = 1.2f;
    const vec3_t *pointsList[2] = { &m_pointA, &m_pointB };

    for (int i = 0; i < 3; ++i)
    {
        if (std::fabsf(m_pointA[i] - m_pointB[i]) > 0.1f) 
        {
            basePoint[i] = m_pointB[i];
            baseFound = true;
            break;
        }
    }

    for (int i = 0; baseFound && i < 3; ++i)
    {
        vec3_t axisVector = { 0.f, 0.f, 0.f }; 
        float diff = basePoint[i] - m_pointB[i];
        if (std::fabsf(diff) < 0.001f) {
            diff = basePoint[i] - m_pointA[i];
        }

        axisVector[i] = 1.0f - 2.0f * (diff > 0.0f);
        vec3_t endPoint = basePoint + axisVector * std::fabsf(diff);
        g_pClientEngfuncs->pEfxAPI->R_BeamPoints(
            basePoint, 
            endPoint,
            m_lineSprite,
            lifeTime * 2.0f, lineWidth, 0,
            lineBrightness, lineSpeed, 0, 0,
            lineColorR, lineColorG, lineColorB
        );
        PrintLineLength(screenWidth, screenHeight, basePoint, endPoint);
    }
}

void CModeMeasurement::LoadLineSprite()
{
    if (m_lineSprite)
        return;

    const char *spritePath = "sprites/laserbeam.spr";
    g_pClientEngfuncs->pfnSPR_Load(spritePath);
    m_lineSprite = g_pClientEngfuncs->pEventAPI->EV_FindModelIndex(spritePath);
}

void CModeMeasurement::Render2D(float frameTime, int screenWidth, int screenHeight, CStringStack &screenText)
{
    if (g_LocalPlayer.PredictionDataValid())
    {
        const vec3_t &originPointA = GetPointOriginA();
        const vec3_t &originPointB = GetPointOriginB();
        float pointsDistance = GetPointsDistance();
        float elevationAngle = GetLineElevationAngle();
        const char *snapModeName = GetSnapModeName();

        const float roundThreshold = 0.08f;
        float fractionalPart = fmodf(pointsDistance, 1.f);
        if (fractionalPart >= (1.f - roundThreshold))
            pointsDistance += (1.f - fractionalPart);
        else if (fractionalPart <= roundThreshold)
            pointsDistance -= fractionalPart;

        screenText.Clear();
        if (originPointA.Length() < 0.0001f)
            screenText.Push("Point A not set");
        else
            screenText.PushPrintf("Point A origin: (%.2f, %.2f, %.2f)",
            originPointA.x, originPointA.y, originPointA.z);

        if (originPointB.Length() < 0.0001f)
            screenText.Push("Point B not set");
        else
            screenText.PushPrintf("Point B origin: (%.2f, %.2f, %.2f)",
            originPointB.x, originPointB.y, originPointB.z);

        screenText.PushPrintf("Points Distance: %.1f (%.3f meters)",
            pointsDistance, pointsDistance / 39.37f);
        screenText.PushPrintf("Elevation Angle: %.2f deg", elevationAngle);
        screenText.PushPrintf("Snap Mode: %s", snapModeName);

        LoadLineSprite();
        if (m_pointA.Length() > 0.0001f && m_pointB.Length() > 0.0001f) {
            DrawVisualization(frameTime, screenWidth, screenHeight);
        }
    }
    else
    {
        screenText.Clear();
        screenText.Push("This mode unavailable when playing demo");
    }

    Utils::DrawStringStack(
        static_cast<int>(ConVars::gsm_margin_right->value),
        static_cast<int>(ConVars::gsm_margin_up->value),
        screenText
    );
}

const char *CModeMeasurement::GetSnapModeName() const
{
    switch (m_snapMode)
    {
    case SnapMode::Free:
        return "Free";
        break;
    case SnapMode::AxisX:
        return "Axis X";
        break;
    case SnapMode::AxisY:
        return "Axis Y";
        break;
    case SnapMode::AxisZ:
        return "Axis Z";
        break;
    case SnapMode::AlongLine:
        return "Along Line";
        break;
    }
    return "";
}

float CModeMeasurement::GetLineElevationAngle() const
{
    vec3_t lineDirection;
    const vec3_t *highPoint;
    const vec3_t *lowPoint;
    
    if (m_pointA.z > m_pointB.z)
    {
        highPoint = &m_pointA;
        lowPoint  = &m_pointB;
    }
    else
    {
        highPoint = &m_pointB;
        lowPoint  = &m_pointA;
    }

    lineDirection = *highPoint - *lowPoint;
    lineDirection = lineDirection.Normalize();
    return asinf(lineDirection.z) * 180.0f / 3.14f;
}
