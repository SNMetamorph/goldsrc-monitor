#include "drawing.h"
#include "core.h"
#include "globals.h"

#define STRING_COUNT		32	
#define STRING_LENGTH		128
#define STRING_HEIGHT		15
#define STRING_MARGIN_RIGHT 400
#define STRING_MARGIN_UP	15
#define SPEEDOMETER_MARGIN	35
static char g_aStrings[STRING_COUNT][STRING_LENGTH];


void DrawModeFull(float time, int screenWidth, int screenHeight)
{
    sprintf_s(g_aStrings[0], STRING_LENGTH, "Velocity: %.2f u/s [%.2f, %.2f, %.2f]", g_pPlayerMove->velocity.Length2D(), g_pPlayerMove->velocity.x, g_pPlayerMove->velocity.y, g_pPlayerMove->velocity.z);
    sprintf_s(g_aStrings[1], STRING_LENGTH, "Origin: [%.2f, %.2f, %.2f]", g_pPlayerMove->origin.x, g_pPlayerMove->origin.y, g_pPlayerMove->origin.z);
    sprintf_s(g_aStrings[2], STRING_LENGTH, "Angles: [%.2f, %.2f, %.2f]", g_pPlayerMove->angles.x, g_pPlayerMove->angles.y, g_pPlayerMove->angles.z);
    sprintf_s(g_aStrings[3], STRING_LENGTH, "Base velocity: [%.2f, %.2f, %.2f]", g_pPlayerMove->basevelocity.x, g_pPlayerMove->basevelocity.y, g_pPlayerMove->basevelocity.z);
    sprintf_s(g_aStrings[4], STRING_LENGTH, "Punch angle: [%.2f, %.2f, %.2f]", g_pPlayerMove->punchangle.x, g_pPlayerMove->punchangle.y, g_pPlayerMove->punchangle.z);
    sprintf_s(g_aStrings[5], STRING_LENGTH, "View offset (Z): %.2f", g_pPlayerMove->view_ofs.z);
    sprintf_s(g_aStrings[6], STRING_LENGTH, "Texture name: %s", g_pPlayerMove->sztexturename);
    sprintf_s(g_aStrings[7], STRING_LENGTH, "Hull type: %d", g_pPlayerMove->usehull);
    sprintf_s(g_aStrings[8], STRING_LENGTH, "Gravity: %.2f", g_pPlayerMove->gravity);
    sprintf_s(g_aStrings[9], STRING_LENGTH, "Friction: %.2f", g_pPlayerMove->friction);
    sprintf_s(g_aStrings[10], STRING_LENGTH, "Max speed: %.2f / client %.2f", g_pPlayerMove->maxspeed, g_pPlayerMove->clientmaxspeed);
    sprintf_s(g_aStrings[11], STRING_LENGTH, "Flags: %x %x %x %x", g_pPlayerMove->flags >> 24, g_pPlayerMove->flags >> 16, g_pPlayerMove->flags >> 8, g_pPlayerMove->flags & 0xFF);
    sprintf_s(g_aStrings[12], STRING_LENGTH, "On ground: %s", g_pPlayerMove->onground != -1 ? "+" : "-");
    sprintf_s(g_aStrings[13], STRING_LENGTH, "Duck time/status: %.2f / %d", g_pPlayerMove->flDuckTime, g_pPlayerMove->bInDuck);
    sprintf_s(g_aStrings[14], STRING_LENGTH, "Time: %.3f seconds", time);
    sprintf_s(g_aStrings[15], STRING_LENGTH, "User variables (f): %.2f / %.2f / %.2f / %.2f", g_pPlayerMove->fuser1, g_pPlayerMove->fuser2, g_pPlayerMove->fuser3, g_pPlayerMove->fuser4);
    sprintf_s(g_aStrings[16], STRING_LENGTH, "User variables (i): %d / %d / %d / %d", g_pPlayerMove->iuser1, g_pPlayerMove->iuser2, g_pPlayerMove->iuser3, g_pPlayerMove->iuser4);

    for (int i = 0; i < STRING_COUNT; ++i)
    {
        g_pClientEngFuncs->pfnDrawString(
            screenWidth - STRING_MARGIN_RIGHT, 
            STRING_MARGIN_UP + (STRING_HEIGHT * i),
            g_aStrings[i], 
            (int)gsm_color_r->value, 
            (int)gsm_color_g->value, 
            (int)gsm_color_b->value
        );
    }
}

void DrawModeSpeedometer(float time, int screenWidth, int screenHeight)
{
    int stringX;
    int stringY;
    int stringWidth;
    float playerSpeed;

    playerSpeed = g_pPlayerMove->velocity.Length2D();
    sprintf_s(g_aStrings[0], STRING_LENGTH, "%.2f", playerSpeed);
    stringWidth = GetStringWidth(g_aStrings[0]);
    stringX     = (screenWidth / 2) - (stringWidth / 2);
    stringY     = (screenHeight / 2) + SPEEDOMETER_MARGIN;

    g_pClientEngFuncs->pfnDrawString(
        stringX, 
        stringY, 
        g_aStrings[0],
        (int)gsm_color_r->value, 
        (int)gsm_color_g->value, 
        (int)gsm_color_b->value
    );
}

void DrawModeAngleTrack(float time, int screenWidth, int screenHeight)
{
    int stringX;
    int stringY;
    int stringWidth;
    float yawVelocity;
    float pitchVelocity;
    static vec3_t lastAngles;
    const float threshold = 0.001f;
    static float trackStartTime;
    static float lastYawVelocity;
    static float lastPitchVelocity;
    vec3_t &currAngles = g_pPlayerMove->angles;

    pitchVelocity   = (currAngles.x - lastAngles.x) / g_pPlayerMove->frametime;
    yawVelocity     = (currAngles.y - lastAngles.y) / g_pPlayerMove->frametime;
    sprintf_s(g_aStrings[0], STRING_LENGTH, "   up : %.2f deg/s", -pitchVelocity);
    sprintf_s(g_aStrings[1], STRING_LENGTH, "right : %.2f deg/s", -yawVelocity);
    stringWidth = GetStringWidth(g_aStrings[0]);
    stringX     = (screenWidth / 2) - (stringWidth / 2);
    stringY     = (screenHeight / 2) + SPEEDOMETER_MARGIN;

    g_pClientEngFuncs->pfnDrawString(
        stringX, 
        stringY, 
        g_aStrings[0],
        (int)gsm_color_r->value, 
        (int)gsm_color_g->value, 
        (int)gsm_color_b->value
    );
    g_pClientEngFuncs->pfnDrawString(
        stringX, 
        stringY + STRING_HEIGHT, 
        g_aStrings[1],
        (int)gsm_color_r->value, 
        (int)gsm_color_g->value, 
        (int)gsm_color_b->value
    );

    // check for start
    if (fabs(lastPitchVelocity) < threshold && fabs(pitchVelocity) > threshold)
        trackStartTime = time;

    if (fabs(pitchVelocity) > threshold)
    {
        g_pClientEngFuncs->Con_Printf("(%.5f; %.2f)\n",
            (time - trackStartTime), -pitchVelocity
        );
    }

    // check for end
    if (fabs(pitchVelocity) < threshold && fabs(lastPitchVelocity) > threshold)
        g_pClientEngFuncs->Con_Printf("\n");

    lastAngles = currAngles;
    lastPitchVelocity = pitchVelocity;
    lastYawVelocity = yawVelocity;
}
