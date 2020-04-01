#include "drawing.h"
#include "core.h"
#include "globals.h"

#define STRING_COUNT		17	
#define STRING_LENGTH		128
#define STRING_HEIGHT		15
#define STRING_MARGIN_RIGHT 400
#define STRING_MARGIN_UP	15
#define SPEEDOMETER_MARGIN	35
static char g_aStrings[STRING_COUNT][STRING_LENGTH];


void DrawModeFull(float time, int screenWidth, int screenHeight)
{
    const int stringCount = 17;
    sprintf_s(g_aStrings[0], STRING_LENGTH, "Velocity: %.2f u/s (%.2f, %.2f, %.2f)", g_pPlayerMove->velocity.Length2D(), g_pPlayerMove->velocity.x, g_pPlayerMove->velocity.y, g_pPlayerMove->velocity.z);
    sprintf_s(g_aStrings[1], STRING_LENGTH, "Origin: (%.2f, %.2f, %.2f)", g_pPlayerMove->origin.x, g_pPlayerMove->origin.y, g_pPlayerMove->origin.z);
    sprintf_s(g_aStrings[2], STRING_LENGTH, "Angles: (%.2f, %.2f, %.2f)", g_pPlayerMove->angles.x, g_pPlayerMove->angles.y, g_pPlayerMove->angles.z);
    sprintf_s(g_aStrings[3], STRING_LENGTH, "Base velocity: (%.2f, %.2f, %.2f)", g_pPlayerMove->basevelocity.x, g_pPlayerMove->basevelocity.y, g_pPlayerMove->basevelocity.z);
    sprintf_s(g_aStrings[4], STRING_LENGTH, "Punch angle: (%.2f, %.2f, %.2f)", g_pPlayerMove->punchangle.x, g_pPlayerMove->punchangle.y, g_pPlayerMove->punchangle.z);
    sprintf_s(g_aStrings[5], STRING_LENGTH, "View offset: (%.2f, %.2f, %.2f)", g_pPlayerMove->view_ofs.x, g_pPlayerMove->view_ofs.y, g_pPlayerMove->view_ofs.z);
    sprintf_s(g_aStrings[6], STRING_LENGTH, "Texture name: %s", g_pPlayerMove->sztexturename);
    sprintf_s(g_aStrings[7], STRING_LENGTH, "Hull type: %d", g_pPlayerMove->usehull);
    sprintf_s(g_aStrings[8], STRING_LENGTH, "Gravity: %.2f", g_pPlayerMove->gravity);
    sprintf_s(g_aStrings[9], STRING_LENGTH, "Friction: %.2f", g_pPlayerMove->friction);
    sprintf_s(g_aStrings[10], STRING_LENGTH, "Max speed: %.2f / client %.2f", g_pPlayerMove->maxspeed, g_pPlayerMove->clientmaxspeed);
    sprintf_s(g_aStrings[11], STRING_LENGTH, "Flags: %d", g_pPlayerMove->flags);
    sprintf_s(g_aStrings[12], STRING_LENGTH, "On ground: %c", g_pPlayerMove->onground != -1 ? '+' : '-');
    sprintf_s(g_aStrings[13], STRING_LENGTH, "Duck time/status: %.2f / %d", g_pPlayerMove->flDuckTime, g_pPlayerMove->bInDuck);
    sprintf_s(g_aStrings[14], STRING_LENGTH, "Time: %.3f seconds", time);
    sprintf_s(g_aStrings[15], STRING_LENGTH, "fuserX: %.2f / %.2f / %.2f / %.2f", g_pPlayerMove->fuser1, g_pPlayerMove->fuser2, g_pPlayerMove->fuser3, g_pPlayerMove->fuser4);
    sprintf_s(g_aStrings[16], STRING_LENGTH, "iuserX: %d / %d / %d / %d", g_pPlayerMove->iuser1, g_pPlayerMove->iuser2, g_pPlayerMove->iuser3, g_pPlayerMove->iuser4);

    for (int i = 0; i < stringCount; ++i)
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
    snprintf(g_aStrings[0], STRING_LENGTH, "%.2f", playerSpeed);
    stringWidth = GetStringWidth(g_aStrings[0]);
    stringX = (screenWidth / 2) - (stringWidth / 2);
    stringY = (screenHeight / 2) + SPEEDOMETER_MARGIN;

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

    pitchVelocity = (currAngles.x - lastAngles.x) / g_pPlayerMove->frametime;
    yawVelocity = (currAngles.y - lastAngles.y) / g_pPlayerMove->frametime;
    snprintf(g_aStrings[0], STRING_LENGTH, "   up : %.2f deg/s", -pitchVelocity);
    snprintf(g_aStrings[1], STRING_LENGTH, "right : %.2f deg/s", -yawVelocity);
    stringWidth = GetStringWidth(g_aStrings[0]);
    stringX = (screenWidth / 2) - (stringWidth / 2);
    stringY = (screenHeight / 2) + SPEEDOMETER_MARGIN;

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

static void TraceViewLine(pmtrace_t *destTraceData)
{
    vec3_t viewDir;
    vec3_t viewAngles;
    vec3_t traceStart;
    vec3_t traceEnd;
    cl_entity_t *localPlayer;
    const float traceDistance = 4096.f;

    g_pClientEngFuncs->GetViewAngles(viewAngles);
    g_pClientEngFuncs->pfnAngleVectors(viewAngles, viewDir, nullptr, nullptr);
    traceStart = g_pPlayerMove->origin + g_pPlayerMove->view_ofs;
    traceEnd = traceStart + (viewDir * traceDistance);
    localPlayer = g_pClientEngFuncs->GetLocalPlayer();

    g_pClientEngFuncs->pEventAPI->EV_SetUpPlayerPrediction(false, true);
    g_pClientEngFuncs->pEventAPI->EV_PushPMStates();
    g_pClientEngFuncs->pEventAPI->EV_SetSolidPlayers(localPlayer->index - 1);
    g_pClientEngFuncs->pEventAPI->EV_SetTraceHull(2);
    g_pClientEngFuncs->pEventAPI->EV_PlayerTrace(
        traceStart, traceEnd, PM_NORMAL,
        localPlayer->index, destTraceData
    );
    g_pClientEngFuncs->pEventAPI->EV_PopPMStates();
}

void DrawModeEntityReport(float time, int screenWidth, int screenHeight)
{
    int entityIndex;
    int stringCount;
    int stringWidth;
    pmtrace_t traceData;
    cl_entity_t *traceEntity;

    TraceViewLine(&traceData);
    if (traceData.fraction < 1.f && traceData.ent > 0)
    {
        entityIndex = g_pClientEngFuncs->pEventAPI->EV_IndexFromTrace(&traceData);
        traceEntity = g_pClientEngFuncs->GetEntityByIndex(entityIndex);

        vec3_t entityOrigin     = traceEntity->curstate.origin;
        vec3_t entityAngles     = traceEntity->curstate.angles;
        vec3_t cameraOrigin     = g_pPlayerMove->origin + g_pPlayerMove->view_ofs;
        float entityDistance    = (entityOrigin - g_pPlayerMove->origin).Length();
        model_t *entityModel    = traceEntity->model;
        stringWidth             = GetStringWidth(entityModel->name) + 100;

        snprintf(g_aStrings[0], STRING_LENGTH, "Entity index: %d", entityIndex);
        snprintf(g_aStrings[1], STRING_LENGTH, "Origin: (%.1f; %.1f; %.1f)",
            entityOrigin.x, entityOrigin.y, entityOrigin.z);
        snprintf(g_aStrings[2], STRING_LENGTH, "Angles: (%.1f; %.1f; %.1f)",
            entityAngles.x, entityAngles.y, entityAngles.z);
        snprintf(g_aStrings[3], STRING_LENGTH, "Distance: %.1f units", entityDistance);
        snprintf(g_aStrings[4], STRING_LENGTH, "Model name: %s", entityModel->name);

        if (entityModel->type != mod_brush)
        {
            snprintf(g_aStrings[5], STRING_LENGTH, "Anim. frame: %.1f",
                traceEntity->curstate.frame);
            snprintf(g_aStrings[6], STRING_LENGTH, "Anim. sequence: %d",
                traceEntity->curstate.sequence);
            snprintf(g_aStrings[7], STRING_LENGTH, "Bodygroup number: %d",
                traceEntity->curstate.body);
            stringCount = 8;
        }
        else
            stringCount = 5;
    }
    else
    {
        strcpy(g_aStrings[0], "Entity not found");
        stringCount = 1;
        stringWidth = 0;
    }

    for (int i = 0; i < stringCount; ++i)
    {
        g_pClientEngFuncs->pfnDrawString(
            screenWidth - max(STRING_MARGIN_RIGHT, stringWidth),
            STRING_MARGIN_UP + (STRING_HEIGHT * i),
            g_aStrings[i],
            (int)gsm_color_r->value,
            (int)gsm_color_g->value,
            (int)gsm_color_b->value
        );
    }
}
