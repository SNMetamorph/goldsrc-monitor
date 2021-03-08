#include "displaymode_entityreport.h"
#include "globals.h"
#include "cvars.h"
#include "util.h"
#include "core.h"
#include "studio.h"
#include <gl/GL.h>

CModeEntityReport &CModeEntityReport::GetInstance()
{
    static CModeEntityReport instance;
    return instance;
}

void CModeEntityReport::Render2D(int scrWidth, int scrHeight)
{
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
        m_iEntityIndex = g_pClientEngFuncs->pEventAPI->EV_IndexFromTrace(&traceData);
    else
        m_iEntityIndex = TraceVisEnt(viewOrigin, viewDir, lineLen * traceData.fraction);

    if (m_iEntityIndex)
    {
        cl_entity_t *traceEntity = g_pClientEngFuncs->GetEntityByIndex(m_iEntityIndex);
        model_t *entityModel = traceEntity->model;

        if (entityModel->type == mod_brush)
            entityOrigin = (entityModel->mins + entityModel->maxs) / 2.f;
        else
        {
            entityOrigin = traceEntity->curstate.origin;
            entityAngles = traceEntity->curstate.angles;
        }

        float entityDistance = (entityOrigin - viewOrigin).Length();        
        g_ScreenText.PushPrintf("Entity Index: %d", m_iEntityIndex);
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

    DrawStringStack(gsm_margin_right->value, gsm_margin_up->value, g_ScreenText);
}

void CModeEntityReport::Render3D()
{
    const float colorR = 0.0f;
    const float colorG = 1.0f;
    const float colorB = 0.0f;

    if (m_iEntityIndex <= 0) 
        return;

    GetEntityBbox(m_iEntityIndex, m_vecBboxMin, m_vecBboxMax);
    vec3_t bboxSize = m_vecBboxMax - m_vecBboxMin;
    vec3_t boxVertices[8] = {
        {m_vecBboxMin.x, m_vecBboxMin.y, m_vecBboxMin.z},
        {m_vecBboxMin.x + bboxSize.x, m_vecBboxMin.y, m_vecBboxMin.z},
        {m_vecBboxMin.x + bboxSize.x, m_vecBboxMin.y, m_vecBboxMin.z + bboxSize.z},
        {m_vecBboxMin.x + bboxSize.x, m_vecBboxMin.y + bboxSize.y, m_vecBboxMin.z + bboxSize.z},
        {m_vecBboxMin.x + bboxSize.x, m_vecBboxMin.y + bboxSize.y, m_vecBboxMin.z},
        {m_vecBboxMin.x, m_vecBboxMin.y + bboxSize.y, m_vecBboxMin.z},
        {m_vecBboxMin.x, m_vecBboxMin.y + bboxSize.y, m_vecBboxMin.z + bboxSize.z},
        {m_vecBboxMin.x, m_vecBboxMin.y, m_vecBboxMin.z + bboxSize.z}
    };

    if (!IsSoftwareRenderer())
    {
        g_pClientEngFuncs->pTriAPI->RenderMode(kRenderNormal);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINE_LOOP);
            glColor3f(colorR, colorG, colorB);
            glVertex3fv(boxVertices[0]); // 1
            glVertex3fv(boxVertices[1]); // 2
            glVertex3fv(boxVertices[2]); // 3
            glVertex3fv(boxVertices[3]); // 4
            glVertex3fv(boxVertices[4]); // 5
            glVertex3fv(boxVertices[5]); // 6
            glVertex3fv(boxVertices[6]); // 7
            glVertex3fv(boxVertices[7]); // 8
            glVertex3fv(boxVertices[0]); // 1
            glVertex3fv(boxVertices[7]); // 8
            glVertex3fv(boxVertices[2]); // 3
            glVertex3fv(boxVertices[1]); // 2
            glVertex3fv(boxVertices[4]); // 5
            glVertex3fv(boxVertices[3]); // 4
            glVertex3fv(boxVertices[6]); // 7
            glVertex3fv(boxVertices[5]); // 6
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
}

int CModeEntityReport::TraceVisEnt(vec3_t &viewOrigin, vec3_t &viewDir, float lineLen)
{
    vec3_t bboxMin;
    vec3_t bboxMax;
    vec3_t lineEnd;
    int entIndex = 0;
    float minFraction = 1.0f;
    
    for (int i = 0; i < g_pPlayerMove->numvisent; ++i)
    {
        physent_t &visEnt = g_pPlayerMove->visents[i];
        vec3_t entDirection = (visEnt.origin - viewOrigin).Normalize();
    
        // skip brush visents
        if (!visEnt.studiomodel)
            continue;

        // skip studiomodel visents which is not visible
        GetEntityBbox(visEnt.info, bboxMin, bboxMax);
        if (!g_pClientEngFuncs->pTriAPI->BoxInPVS(bboxMin, bboxMax))
            continue;

        // check for intersection
        lineEnd = viewOrigin + (viewDir * lineLen);
        float traceFraction = TraceBBoxLine(bboxMin, bboxMax, viewOrigin, lineEnd);
        if (traceFraction < minFraction)
        {
            entIndex    = visEnt.info;
            minFraction = traceFraction;
        }
    }

    return entIndex;
}

void CModeEntityReport::GetEntityBbox(int entityIndex, vec3_t &bboxMin, vec3_t &bboxMax)
{ 
    int seqIndex;
    cl_entity_t *entTarget;
    studiohdr_t *mdlHeader;
    mstudioseqdesc_t *seqDesc;
    
    entTarget = g_pClientEngFuncs->GetEntityByIndex(entityIndex);
    if (entTarget->model && entTarget->model->type == mod_studio)
    {
        vec3_t &entOrigin = entTarget->curstate.origin;
        mdlHeader = (studiohdr_t *)entTarget->model->cache.data;
        seqDesc = (mstudioseqdesc_t*)((char *)mdlHeader + mdlHeader->seqindex);
        seqIndex = entTarget->curstate.sequence;

        bboxMin = entOrigin + seqDesc[seqIndex].bbmin;
        bboxMax = entOrigin + seqDesc[seqIndex].bbmax;
    }
    else
    {
        bboxMin = entTarget->curstate.mins;
        bboxMax = entTarget->curstate.maxs;
    }
}
