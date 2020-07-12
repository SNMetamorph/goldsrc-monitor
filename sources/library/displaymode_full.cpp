#include "displaymode_full.h"
#include "globals.h"
#include "core.h"
#include "util.h"

CModeFull &g_ModeFull = CModeFull::GetInstance();

CModeFull &CModeFull::GetInstance()
{
    static CModeFull instance;
    return instance;
}

void CModeFull::Render2D(int scrWidth, int scrHeight)
{
    float frameTime             = GetSmoothFrameTime();
    float velocityNum           = g_pPlayerMove->velocity.Length2D();
    const vec3_t &origin        = g_pPlayerMove->origin;
    const vec3_t &velocity      = g_pPlayerMove->velocity;
    const vec3_t &angles        = g_pPlayerMove->angles;
    const vec3_t &baseVelocity  = g_pPlayerMove->basevelocity;
    const vec3_t &punchAngle    = g_pPlayerMove->punchangle;
    const vec3_t &viewOffset    = g_pPlayerMove->view_ofs;

    g_ScreenText.Clear();
    g_ScreenText.PushPrintf("FPS: %.1f", 1.f / frameTime);
    g_ScreenText.PushPrintf("Time: %.2f seconds", g_pClientEngFuncs->GetClientTime());
    g_ScreenText.PushPrintf("Frame Time: %.1f ms\n", frameTime * 1000.f);

    g_ScreenText.PushPrintf("Velocity: %.2f u/s (%.2f, %.2f, %.2f)", velocityNum, velocity.x, velocity.y, velocity.z);
    g_ScreenText.PushPrintf("Origin: (%.2f, %.2f, %.2f)", origin.x, origin.y, origin.z);
    g_ScreenText.PushPrintf("Angles: (%.2f, %.2f, %.2f)", angles.x, angles.y, angles.z);
    g_ScreenText.PushPrintf("Base Velocity: (%.2f, %.2f, %.2f)", baseVelocity.x, baseVelocity.y, baseVelocity.z);
    g_ScreenText.PushPrintf("Max Velocity: %.2f (client %.2f)\n", g_pPlayerMove->maxspeed, g_pPlayerMove->clientmaxspeed);

    g_ScreenText.PushPrintf("View Offset: (%.2f, %.2f, %.2f)", viewOffset.x, viewOffset.y, viewOffset.z);
    g_ScreenText.PushPrintf("Punch Angle: (%.2f, %.2f, %.2f)", punchAngle.x, punchAngle.y, punchAngle.z);
    g_ScreenText.PushPrintf("Duck Time: %.2f", g_pPlayerMove->flDuckTime);
    g_ScreenText.PushPrintf("In Duck Process: %c", g_pPlayerMove->bInDuck ? '+' : '-');
    g_ScreenText.PushPrintf("Player Flags: %d", g_pPlayerMove->flags);
    g_ScreenText.PushPrintf("Hull Type: %d", g_pPlayerMove->usehull);
    g_ScreenText.PushPrintf("Gravity: %.2f", g_pPlayerMove->gravity);
    g_ScreenText.PushPrintf("Friction: %.2f", g_pPlayerMove->friction);
    g_ScreenText.PushPrintf("On Ground: %c", g_pPlayerMove->onground != -1 ? '+' : '-');
    g_ScreenText.PushPrintf("Texture Name: %s", g_pPlayerMove->sztexturename);
    g_ScreenText.PushPrintf("fuserX: %.2f / %.2f / %.2f / %.2f", 
        g_pPlayerMove->fuser1, 
        g_pPlayerMove->fuser2, 
        g_pPlayerMove->fuser3, 
        g_pPlayerMove->fuser4
    );
    g_ScreenText.PushPrintf("iuserX: %d / %d / %d / %d", 
        g_pPlayerMove->iuser1, 
        g_pPlayerMove->iuser2, 
        g_pPlayerMove->iuser3, 
        g_pPlayerMove->iuser4
    );

    DrawStringStack(400, 15, g_ScreenText);
}

float CModeFull::GetSmoothFrameTime()
{
    static float lastSysTime;
    static float frameTime      = 0;
    static float lastFrameTime  = 0;
    const float smoothFactor    = 0.24f;
    const float diffThreshold   = 0.13f;
    float currSysTime           = GetCurrentSysTime();
    float timeDelta             = currSysTime - lastSysTime;

    if ((timeDelta - lastFrameTime) > diffThreshold)
        timeDelta = lastFrameTime;

    frameTime       += (timeDelta - frameTime) * smoothFactor;
    lastFrameTime   = frameTime;
    lastSysTime     = currSysTime;
    return frameTime;
}
