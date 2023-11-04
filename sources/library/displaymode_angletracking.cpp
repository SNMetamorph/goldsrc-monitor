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

#include "displaymode_angletracking.h"
#include "application.h"
#include "client_module.h"
#include "local_player.h"

CModeAngleTracking::CModeAngleTracking(const CLocalPlayer &playerRef)
    : m_localPlayer(playerRef)
{
    m_lastAngles = Vector(0.0f, 0.0f, 0.0f);
    m_trackStartTime = 0.0f;
    m_lastYawVelocity = 0.0f;
    m_lastPitchVelocity = 0.0f;
}

void CModeAngleTracking::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    if (!m_localPlayer.PredictionDataValid())
        return;

    const float threshold = 0.001f;
    const vec3_t &currAngles = m_localPlayer.GetAngles();
    float pitchVelocity = (currAngles.x - m_lastAngles.x) / frameTime;
    float yawVelocity = (currAngles.y - m_lastAngles.y) / frameTime;

    screenText.Clear();
    screenText.PushPrintf("   up : %.2f deg/s", -pitchVelocity);
    screenText.PushPrintf("right : %.2f deg/s", -yawVelocity);

    const int stringWidth = Utils::GetStringWidth(screenText.StringAt(0));
    const int marginDown = 35;
    Utils::DrawStringStack(
        (scrWidth / 2) + stringWidth / 2, 
        (scrHeight / 2) + marginDown, 
        screenText
    );

    // check for start
    if (fabs(m_lastPitchVelocity) < threshold && fabs(pitchVelocity) > threshold) {
        m_trackStartTime = g_pClientEngfuncs->GetClientTime();
    }

    if (fabs(pitchVelocity) > threshold)
    {
        g_pClientEngfuncs->Con_Printf("(%.5f; %.2f)\n",
            (g_pClientEngfuncs->GetClientTime() - m_trackStartTime), -pitchVelocity
        );
    }

    // check for end
    if (fabs(pitchVelocity) < threshold && fabs(m_lastPitchVelocity) > threshold) {
        g_pClientEngfuncs->Con_Printf("\n");
    }

    m_lastAngles = currAngles;
    m_lastPitchVelocity = pitchVelocity;
    m_lastYawVelocity = yawVelocity;
}
