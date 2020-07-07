#include "drawing.h"
#include "util.h"
#include "core.h"
#include "globals.h"
#include "string_stack.h"
#include "measurement.h"
#include "studio.h"

#define STRING_COUNT		20	
#define STRING_LENGTH		128
#define STRING_HEIGHT		15
#define STRING_MARGIN_RIGHT 400
#define STRING_MARGIN_UP	15
#define SPEEDOMETER_MARGIN	35
static char g_aStrings[STRING_COUNT][STRING_LENGTH];
static CStringStack g_ScreenText(&g_aStrings[0][0], STRING_LENGTH, STRING_COUNT);

static float GetSmoothFrameTime()
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

static void DrawStringStack(int screenWidth, const CStringStack &stringStack)
{
    int linesSkipped = 0;
    int maxStringWidth = 0;
    int stringCount = stringStack.GetStringCount();

    for (int i = 0; i < stringCount; ++i)
    {
        const char *textString = stringStack.StringAt(i);
        int stringWidth = GetStringWidth(textString);
        if (stringWidth > maxStringWidth)
            maxStringWidth = stringWidth;
    }

    for (int i = 0; i < stringCount; ++i)
    {
        const char *textString = stringStack.StringAt(i);
        g_pClientEngFuncs->pfnDrawString(
            screenWidth - max(STRING_MARGIN_RIGHT, maxStringWidth + 5),
            STRING_MARGIN_UP + (STRING_HEIGHT * (i + linesSkipped)),
            textString,
            (int)gsm_color_r->value,
            (int)gsm_color_g->value,
            (int)gsm_color_b->value
        );

        if (textString[strlen(textString) - 1] == '\n')
            ++linesSkipped;
    }
}

void DrawModeFull(float time, int screenWidth, int screenHeight)
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
    g_ScreenText.PushPrintf("Time: %.2f seconds", time);
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

    DrawStringStack(screenWidth, g_ScreenText);
}

void DrawModeSpeedometer(float time, int screenWidth, int screenHeight)
{
    int stringWidth;
    float playerSpeed;
    char *textString = g_aStrings[0];

    playerSpeed = g_pPlayerMove->velocity.Length2D();
    snprintf(textString, STRING_LENGTH, "%.2f", playerSpeed);
    stringWidth = GetStringWidth(textString);

    g_pClientEngFuncs->pfnDrawString(
        (screenWidth / 2) - (stringWidth / 2),
        (screenHeight / 2) + SPEEDOMETER_MARGIN,
        textString,
        (int)gsm_color_r->value,
        (int)gsm_color_g->value,
        (int)gsm_color_b->value
    );
}

void DrawModeAngleTrack(float time, int screenWidth, int screenHeight)
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

    for (int i = 0; i < g_ScreenText.GetStringCount(); ++i)
    {
        g_pClientEngFuncs->pfnDrawString(
            (screenWidth / 2) - (stringWidth / 2),
            (screenHeight / 2) + SPEEDOMETER_MARGIN + (STRING_HEIGHT * i),
            g_ScreenText.StringAt(i),
            (int)gsm_color_r->value,
            (int)gsm_color_g->value,
            (int)gsm_color_b->value
        );
    }

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

static bool TraceVisEnt(vec3_t &viewOrigin, vec3_t &viewDir, float lineLen, int &entIndex)
{
    vec3_t bboxMin;
    vec3_t bboxMax;
    vec3_t lineEnd;
    cl_entity_t *traceEntity;
    studiohdr_t *mdlHeader;
    mstudioseqdesc_t *seqDesc;
    float minFraction = 1.0f;

    for (int i = 1; i < g_pPlayerMove->numvisent; ++i)
    {
        physent_t &visEnt = g_pPlayerMove->visents[i];
        vec3_t entDirection = (visEnt.origin - viewOrigin).Normalize();

        // skip studiomodel visents which is not in view cone
        if (visEnt.studiomodel && DotProduct(entDirection, viewDir) < 0.95f)
            continue;

        traceEntity = g_pClientEngFuncs->GetEntityByIndex(visEnt.info);
        if (traceEntity->model && traceEntity->model->type == mod_studio)
            mdlHeader = (studiohdr_t *)traceEntity->model->cache.data;
        else
            mdlHeader = nullptr;

        if (mdlHeader)
        {
            seqDesc = (mstudioseqdesc_t*)((char *)mdlHeader + mdlHeader->seqindex);
            bboxMin = visEnt.origin + seqDesc[traceEntity->curstate.sequence].bbmin;
            bboxMax = visEnt.origin + seqDesc[traceEntity->curstate.sequence].bbmax;

            // check for intersection
            lineEnd = viewOrigin + (viewDir * lineLen);
            float traceFraction = TraceBBoxLine(bboxMin, bboxMax, viewOrigin, lineEnd);
            if (traceFraction < minFraction)
            {
                minFraction = traceFraction;
                entIndex = visEnt.info;
            }
        }
    }

    if (minFraction < 1.0f)
        return true;

    entIndex = 0;
    return false;
}

void DrawModeEntityReport(float time, int screenWidth, int screenHeight)
{
    int entityIndex;
    vec3_t viewDir;
    vec3_t viewOrigin;
    vec3_t viewAngles;
    vec3_t entityOrigin;
    vec3_t entityAngles;
    pmtrace_t traceData;
    const float lineLen = 4096.f;

    g_ScreenText.Clear();
    g_pClientEngFuncs->GetViewAngles(viewAngles);
    g_pClientEngFuncs->pfnAngleVectors(viewAngles, viewDir, nullptr, nullptr);
    viewOrigin = g_pPlayerMove->origin + g_pPlayerMove->view_ofs;
    TraceLine(viewOrigin, viewDir, lineLen, &traceData);

    if (traceData.fraction < 1.f && traceData.ent > 0)
        entityIndex = g_pClientEngFuncs->pEventAPI->EV_IndexFromTrace(&traceData);
    else
        TraceVisEnt(viewOrigin, viewDir, lineLen * traceData.fraction, entityIndex);

    if (entityIndex)
    {
        cl_entity_t *traceEntity = g_pClientEngFuncs->GetEntityByIndex(entityIndex);
        model_t *entityModel = traceEntity->model;
        
        if (entityModel->type == mod_brush)
            entityOrigin = (entityModel->mins + entityModel->maxs) / 2.f;
        else
        {
            entityOrigin = traceEntity->curstate.origin;
            entityAngles = traceEntity->curstate.angles;
        }

        float entityDistance = (entityOrigin - viewOrigin).Length();        
        g_ScreenText.PushPrintf("Entity Index: %d", entityIndex);
        g_ScreenText.PushPrintf("Origin: (%.1f; %.1f; %.1f)",
            entityOrigin.x, entityOrigin.y, entityOrigin.z);
        g_ScreenText.PushPrintf("Distance: %.1f units", entityDistance);

        if (entityModel->type != mod_brush)
        {
            g_ScreenText.PushPrintf("Angles: (%.1f; %.1f; %.1f)",
                entityAngles.x, entityAngles.y, entityAngles.z);
        }
        else
        {
            vec3_t brushSize = entityModel->maxs - entityModel->mins;
            g_ScreenText.PushPrintf("Brush Size: (%.1f; %.1f; %.1f)",
                brushSize.x, brushSize.y, brushSize.z);
        }

        if (entityModel->type != mod_brush)
        {
            g_ScreenText.PushPrintf("Model Name: %s", entityModel->name);
            g_ScreenText.PushPrintf("Anim. Frame: %.1f",
                traceEntity->curstate.frame);
            g_ScreenText.PushPrintf("Anim. Sequence: %d",
                traceEntity->curstate.sequence);
            g_ScreenText.PushPrintf("Bodygroup Number: %d",
                traceEntity->curstate.body);
            g_ScreenText.PushPrintf("Skin Number: %d",
                traceEntity->curstate.skin);
        }
    }
    else
        g_ScreenText.Push("Entity not found");

    DrawStringStack(screenWidth, g_ScreenText);
}

void DrawModeMeasurement(float time, int screenWidth, int screenHeight)
{
    vec3_t originPointA = g_Measurement.GetPointOriginA();
    vec3_t originPointB = g_Measurement.GetPointOriginB();
    float pointsDistance = g_Measurement.GetPointsDistance();
    float elevationAngle = g_Measurement.GetLineElevationAngle();
    const char *snapModeName = g_Measurement.GetSnapModeName();

    const float roundThreshold = 0.935f;
    if (fmodf(pointsDistance, 1.f) >= roundThreshold)
        pointsDistance += (1.f - roundThreshold);

    g_ScreenText.Clear();
    if (originPointA.Length() < 0.0001f)
        g_ScreenText.Push("Point A not set");
    else
        g_ScreenText.PushPrintf("Point A origin: (%.2f, %.2f, %.2f)", 
            originPointA.x, originPointA.y, originPointA.z);

    if (originPointB.Length() < 0.0001f)
        g_ScreenText.Push("Point B not set");
    else
        g_ScreenText.PushPrintf("Point B origin: (%.2f, %.2f, %.2f)", 
            originPointB.x, originPointB.y, originPointB.z);

    g_ScreenText.PushPrintf("Points Distance: %.1f (%.2f meters)",
        pointsDistance, pointsDistance / 39.37f);
    g_ScreenText.PushPrintf("Elevation Angle: %.2f deg", elevationAngle);
    g_ScreenText.PushPrintf("Snap Mode: %s", snapModeName);

    DrawStringStack(screenWidth, g_ScreenText);
    g_Measurement.Visualize(screenWidth, screenHeight);
}
