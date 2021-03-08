#include "displaymode_angletracking.h"
#include "core.h"
#include "globals.h"

CModeAngleTracking &CModeAngleTracking::GetInstance()
{
    static CModeAngleTracking instance;
    return instance;
}

void CModeAngleTracking::Render2D(int scrWidth, int scrHeight)
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

    g_ScreenText.Clear();
    g_ScreenText.PushPrintf("   up : %.2f deg/s", -pitchVelocity);
    g_ScreenText.PushPrintf("right : %.2f deg/s", -yawVelocity);
    stringWidth = GetStringWidth(g_ScreenText.StringAt(0));

    const int marginDown = 35;
    DrawStringStack(
        (scrWidth / 2) + stringWidth / 2, 
        (scrHeight / 2) + marginDown, 
        g_ScreenText
    );

    // check for start
    if (fabs(lastPitchVelocity) < threshold && fabs(pitchVelocity) > threshold)
        trackStartTime = g_pClientEngFuncs->GetClientTime();

    if (fabs(pitchVelocity) > threshold)
    {
        g_pClientEngFuncs->Con_Printf("(%.5f; %.2f)\n",
            (g_pClientEngFuncs->GetClientTime() - trackStartTime), -pitchVelocity
        );
    }

    // check for end
    if (fabs(pitchVelocity) < threshold && fabs(lastPitchVelocity) > threshold)
        g_pClientEngFuncs->Con_Printf("\n");

    lastAngles = currAngles;
    lastPitchVelocity = pitchVelocity;
    lastYawVelocity = yawVelocity;
}
