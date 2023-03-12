#include "displaymode_angletracking.h"
#include "application.h"
#include "client_module.h"
#include "local_player.h"

CModeAngleTracking::CModeAngleTracking()
{
    m_vecLastAngles = Vector(0.0f, 0.0f, 0.0f);
    m_flTrackStartTime = 0.0f;
    m_flLastYawVelocity = 0.0f;
    m_flLastPitchVelocity = 0.0f;
}

void CModeAngleTracking::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    if (!g_LocalPlayer.PlayerMoveAvailable())
        return;

    const float threshold = 0.001f;
    const vec3_t &currAngles = g_LocalPlayer.GetAngles();
    float pitchVelocity = (currAngles.x - m_vecLastAngles.x) / frameTime;
    float yawVelocity = (currAngles.y - m_vecLastAngles.y) / frameTime;

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
    if (fabs(m_flLastPitchVelocity) < threshold && fabs(pitchVelocity) > threshold) {
        m_flTrackStartTime = g_pClientEngfuncs->GetClientTime();
    }

    if (fabs(pitchVelocity) > threshold)
    {
        g_pClientEngfuncs->Con_Printf("(%.5f; %.2f)\n",
            (g_pClientEngfuncs->GetClientTime() - m_flTrackStartTime), -pitchVelocity
        );
    }

    // check for end
    if (fabs(pitchVelocity) < threshold && fabs(m_flLastPitchVelocity) > threshold) {
        g_pClientEngfuncs->Con_Printf("\n");
    }

    m_vecLastAngles = currAngles;
    m_flLastPitchVelocity = pitchVelocity;
    m_flLastYawVelocity = yawVelocity;
}
