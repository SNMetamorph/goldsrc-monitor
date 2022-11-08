#include "displaymode_full.h"
#include "client_module.h"
#include "cvars.h"
#include "utils.h"
#include "local_player.h"

CModeFull::CModeFull()
{
}

void CModeFull::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    float frameTime             = GetSmoothFrameTime();
    float velocityNum           = g_LocalPlayer.GetVelocityHorz();
    const vec3_t &origin        = g_LocalPlayer.GetOrigin();
    const vec3_t &velocity      = g_LocalPlayer.GetVelocity();
    const vec3_t &angles        = g_LocalPlayer.GetAngles();
    const vec3_t &baseVelocity  = g_LocalPlayer.GetBaseVelocity();
    const vec3_t &punchAngle    = g_LocalPlayer.GetPunchAngles();
    const vec3_t &viewOffset    = g_LocalPlayer.GetViewOffset();

    screenText.Clear();
    screenText.PushPrintf("FPS: %.1f", 1.f / frameTime);
    screenText.PushPrintf("Time: %.2f seconds", g_pClientEngfuncs->GetClientTime());
    screenText.PushPrintf("Frame Time: %.1f ms\n", frameTime * 1000.f);

    screenText.PushPrintf("Velocity: %.2f u/s (%.2f, %.2f, %.2f)", velocityNum, velocity.x, velocity.y, velocity.z);
    screenText.PushPrintf("Origin: (%.2f, %.2f, %.2f)", origin.x, origin.y, origin.z);
    screenText.PushPrintf("Angles: (%.2f, %.2f, %.2f)", angles.x, angles.y, angles.z);
    screenText.PushPrintf("Base Velocity: (%.2f, %.2f, %.2f)", baseVelocity.x, baseVelocity.y, baseVelocity.z);
    screenText.PushPrintf("Max Velocity: %.2f (client %.2f)", g_pPlayerMove->maxspeed, g_pPlayerMove->clientmaxspeed);
    screenText.PushPrintf("Movetype: %s\n", Utils::GetMovetypeName(g_pPlayerMove->movetype));

    screenText.PushPrintf("View Offset: (%.2f, %.2f, %.2f)", viewOffset.x, viewOffset.y, viewOffset.z);
    screenText.PushPrintf("Punch Angle: (%.2f, %.2f, %.2f)", punchAngle.x, punchAngle.y, punchAngle.z);
    screenText.PushPrintf("Duck Time: %.2f", g_pPlayerMove->flDuckTime);
    screenText.PushPrintf("In Duck Process: %s", g_pPlayerMove->bInDuck ? "yes" : "no");
    screenText.PushPrintf("Player Flags: %d", g_pPlayerMove->flags);
    screenText.PushPrintf("Hull Type: %d", g_pPlayerMove->usehull);
    screenText.PushPrintf("Gravity: %.2f", g_pPlayerMove->gravity);
    screenText.PushPrintf("Friction: %.2f", g_pPlayerMove->friction);
    screenText.PushPrintf("On Ground: %s", g_pPlayerMove->onground != -1 ? "yes" : "no");
    screenText.PushPrintf("Texture Name: %s", g_pPlayerMove->sztexturename);
    screenText.PushPrintf("fuserX: %.2f / %.2f / %.2f / %.2f",
        g_pPlayerMove->fuser1, 
        g_pPlayerMove->fuser2, 
        g_pPlayerMove->fuser3, 
        g_pPlayerMove->fuser4
    );
    screenText.PushPrintf("iuserX: %d / %d / %d / %d",
        g_pPlayerMove->iuser1, 
        g_pPlayerMove->iuser2, 
        g_pPlayerMove->iuser3, 
        g_pPlayerMove->iuser4
    );

    Utils::DrawStringStack(
        static_cast<int>(ConVars::gsm_margin_right->value),
        static_cast<int>(ConVars::gsm_margin_up->value),
        screenText
    );
}
