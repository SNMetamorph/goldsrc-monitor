#include "displaymode_entityreport.h"
#include "globals.h"
#include "util.h"
#include "core.h"
#include "studio.h"
#include <gl/GL.h>

CModeEntityReport &g_ModeEntityReport = CModeEntityReport::GetInstance();

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
        TraceVisEnt(viewOrigin, viewDir, lineLen * traceData.fraction);

    if (m_iEntityIndex)
    {
        cl_entity_t *traceEntity = g_pClientEngFuncs->GetEntityByIndex(m_iEntityIndex);
        model_t *entityModel = traceEntity->model;

        if (entityModel->type == mod_brush)
        {
            entityOrigin = (entityModel->mins + entityModel->maxs) / 2.f;
            m_vecBboxMin = entityModel->mins;
            m_vecBboxMax = entityModel->maxs;
        }
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

    DrawStringStack(400, 15, g_ScreenText);
}

void CModeEntityReport::Render3D()
{
    const float colorR = 0.f;
    const float colorG = 1.f;
    const float colorB = 0.f;
    const float transparency = 0.7f;

    if (IsSoftwareRenderer())
        return;

    if (m_iEntityIndex != 0)
    {
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

        g_pClientEngFuncs->pTriAPI->RenderMode(kRenderTransAlpha);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINE_LOOP);

        glColor4f(colorR, colorG, colorB, transparency);
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
        g_pClientEngFuncs->pTriAPI->RenderMode(kRenderNormal);
    }
}

bool CModeEntityReport::TraceVisEnt(vec3_t &viewOrigin, vec3_t &viewDir, float lineLen)
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
                minFraction     = traceFraction;
                m_iEntityIndex  = visEnt.info;
                m_vecBboxMin    = bboxMin;
                m_vecBboxMax    = bboxMax;
            }
        }
    }

    if (minFraction < 1.0f)
        return true;

    m_iEntityIndex = 0;
    return false;
}
