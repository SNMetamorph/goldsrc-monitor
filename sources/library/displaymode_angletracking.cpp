#include "displaymode_angletracking.h"
#include "application.h"
#include "client_module.h"

CModeAngleTracking &CModeAngleTracking::GetInstance()
{
    static CModeAngleTracking instance;
    return instance;
}

void CModeAngleTracking::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    int stringWidth;
    float yawVelocity;
    float pitchVelocity;
    static vec3_t lastAngles;
    const float threshold = 0.001f;
    static float trackStartTime;
    static float lastYawVelocity;
    static float lastPitchVelocity;
    vec3_t &currAngles = g_pPlayerMove->angles;

    pitchVelocity = (currAngles.x - lastAngles.x) / g_pPlayerMove->frametime;
    yawVelocity = (currAngles.y - lastAngles.y) / g_pPlayerMove->frametime;

    screenText.Clear();
    screenText.PushPrintf("   up : %.2f deg/s", -pitchVelocity);
    screenText.PushPrintf("right : %.2f deg/s", -yawVelocity);
    stringWidth = Utils::GetStringWidth(screenText.StringAt(0));

    const int marginDown = 35;
    Utils::DrawStringStack(
        (scrWidth / 2) + stringWidth / 2, 
        (scrHeight / 2) + marginDown, 
        screenText
    );

    // check for start
    if (fabs(lastPitchVelocity) < threshold && fabs(pitchVelocity) > threshold)
        trackStartTime = g_pClientEngfuncs->GetClientTime();

    if (fabs(pitchVelocity) > threshold)
    {
        g_pClientEngfuncs->Con_Printf("(%.5f; %.2f)\n",
            (g_pClientEngfuncs->GetClientTime() - trackStartTime), -pitchVelocity
        );
    }

    // check for end
    if (fabs(pitchVelocity) < threshold && fabs(lastPitchVelocity) > threshold)
        g_pClientEngfuncs->Con_Printf("\n");

    lastAngles = currAngles;
    lastPitchVelocity = pitchVelocity;
    lastYawVelocity = yawVelocity;
}
