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

#include "displaymode_full.h"
#include "client_module.h"
#include "cvars.h"
#include "utils.h"
#include "local_player.h"

CModeFull::CModeFull(const CLocalPlayer &playerRef)
    : m_localPlayer(playerRef)
{
    m_frameTime = 0.0f;
    m_lastFrameTime = 0.0f;
    m_lastSysTime = 0.0f;
}

void CModeFull::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    if (m_localPlayer.PredictionDataValid())
    {
        float timeDelta = GetSmoothSystemFrametime();
        float velocityNum = m_localPlayer.GetVelocity().Length2D();
        const vec3_t &origin = m_localPlayer.GetOrigin();
        const vec3_t &velocity = m_localPlayer.GetVelocity();
        const vec3_t &angles = m_localPlayer.GetAngles();
        const vec3_t &baseVelocity = m_localPlayer.GetBaseVelocity();
        const vec3_t &punchAngle = m_localPlayer.GetPunchAngles();
        const vec3_t &viewOffset = m_localPlayer.GetViewOffset();

        screenText.Clear();
        screenText.PushPrintf("FPS: %.1f", 1.f / timeDelta);
        screenText.PushPrintf("Time: %.2f seconds", g_pClientEngfuncs->GetClientTime());
        screenText.PushPrintf("Frame Time: %.1f ms\n", timeDelta * 1000.f);

        screenText.PushPrintf("Velocity: %.2f u/s (%.2f, %.2f, %.2f)", velocityNum, velocity.x, velocity.y, velocity.z);
        screenText.PushPrintf("Origin: (%.2f, %.2f, %.2f)", origin.x, origin.y, origin.z);
        screenText.PushPrintf("Angles: (%.2f, %.2f, %.2f)", angles.x, angles.y, angles.z);
        screenText.PushPrintf("Base Velocity: (%.2f, %.2f, %.2f)", baseVelocity.x, baseVelocity.y, baseVelocity.z);
        screenText.PushPrintf("Max Velocity: %.2f (client %.2f)", m_localPlayer.GetMaxSpeed(), m_localPlayer.GetClientMaxSpeed());
        screenText.PushPrintf("Movetype: %s\n", Utils::GetMovetypeName(m_localPlayer.GetMovetype()));

        screenText.PushPrintf("View Offset: (%.2f, %.2f, %.2f)", viewOffset.x, viewOffset.y, viewOffset.z);
        screenText.PushPrintf("Punch Angle: (%.2f, %.2f, %.2f)", punchAngle.x, punchAngle.y, punchAngle.z);
        screenText.PushPrintf("Duck Time: %.2f", m_localPlayer.GetDuckTime());
        screenText.PushPrintf("In Duck Process: %s", m_localPlayer.IsDucking() ? "yes" : "no");
        screenText.PushPrintf("Player Flags: %d", m_localPlayer.GetFlags());
        screenText.PushPrintf("Hull Type: %d", m_localPlayer.GetHullType());
        screenText.PushPrintf("Gravity: %.2f", m_localPlayer.GetGravity());
        screenText.PushPrintf("Friction: %.2f", m_localPlayer.GetFriction());
        screenText.PushPrintf("On Ground: %s", m_localPlayer.OnGround() ? "yes" : "no");
        screenText.PushPrintf("fuserX: %.2f / %.2f / %.2f / %.2f",
            m_localPlayer.GetFloatUserVar(1),
            m_localPlayer.GetFloatUserVar(2),
            m_localPlayer.GetFloatUserVar(3),
            m_localPlayer.GetFloatUserVar(4)
        );
        screenText.PushPrintf("iuserX: %d / %d / %d / %d",
            m_localPlayer.GetIntUserVar(1),
            m_localPlayer.GetIntUserVar(2),
            m_localPlayer.GetIntUserVar(3),
            m_localPlayer.GetIntUserVar(4)
        );
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

float CModeFull::GetSmoothSystemFrametime()
{
    const float smoothFactor    = 0.24f;
    const float diffThreshold   = 0.13f;
    float currSysTime           = Utils::GetCurrentSysTime();
    float timeDelta             = currSysTime - m_lastSysTime;

    if ((timeDelta - m_lastFrameTime) > diffThreshold)
        timeDelta = m_lastFrameTime;

    m_frameTime       += (timeDelta - m_frameTime) * smoothFactor;
    m_lastFrameTime   = m_frameTime;
    m_lastSysTime     = currSysTime;
    return m_frameTime;
}
