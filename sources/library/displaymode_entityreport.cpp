#include "displaymode_entityreport.h"
#include "cvars.h"
#include "utils.h"
#include "client_module.h"
#include "engine_module.h"
#include "studio.h"
#include "entity_dictionary.h"
#include <gl/GL.h>
#include <algorithm>
#include <iterator> 

CModeEntityReport &CModeEntityReport::GetInstance()
{
    static CModeEntityReport instance;
    return instance;
}

void CModeEntityReport::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    vec3_t entityOrigin;
    vec3_t entityAngles;

    if (!g_EntityDictionary.IsInitialized())
        g_EntityDictionary.Initialize();

    screenText.Clear();
    m_iEntityIndex = TraceEntity();
    if (m_iEntityIndex)
    {
        CEntityDescription entityDesc;
        cl_entity_t *traceEntity = g_pClientEngfuncs->GetEntityByIndex(m_iEntityIndex);
        model_t *entityModel = traceEntity->model;
        bool isDescFound = g_EntityDictionary.FindDescription(m_iEntityIndex, entityDesc);
        entityAngles = traceEntity->curstate.angles;

        if (entityModel->type == mod_brush)
            entityOrigin = (entityModel->mins + entityModel->maxs) / 2.f;   
        else
            entityOrigin = traceEntity->curstate.origin;

        screenText.PushPrintf("Entity Index: %d", m_iEntityIndex);
        screenText.PushPrintf("Origin: (%.1f; %.1f; %.1f)",
            entityOrigin.x, entityOrigin.y, entityOrigin.z);
        screenText.PushPrintf("Distance: %.1f units",
            GetEntityDistance(m_iEntityIndex));
        screenText.PushPrintf("Angles: (%.1f; %.1f; %.1f)",
            entityAngles.x, entityAngles.y, entityAngles.z);
        
        if (entityModel->type == mod_brush)
        {
            vec3_t brushSize = entityModel->maxs - entityModel->mins;
            screenText.PushPrintf("Brush Size: (%.1f; %.1f; %.1f)",
                brushSize.x, brushSize.y, brushSize.z);
        }

        if (entityModel->type != mod_brush)
        {
            entityModel = g_pClientEngfuncs->hudGetModelByIndex(traceEntity->curstate.modelindex);
            screenText.PushPrintf("Model Name: %s", entityModel->name);
            screenText.PushPrintf("Anim. Frame: %.1f",
                traceEntity->curstate.frame);
            screenText.PushPrintf("Anim. Sequence: %d",
                traceEntity->curstate.sequence);
            screenText.PushPrintf("Bodygroup Number: %d",
                traceEntity->curstate.body);
            screenText.PushPrintf("Skin Number: %d",
                traceEntity->curstate.skin);
        }

        if (isDescFound)
        {
            std::string entityProp;
            screenText.Push("Entity properties");
            for (int i = 0; i < entityDesc.GetKeyValueCount(); ++i)
            {
                entityDesc.GetKeyValueString(i, entityProp);
                screenText.PushPrintf("    %s", entityProp.c_str());
            }
        }
        else
            screenText.Push("Properties info not found");
    }
    else
        screenText.Push("Entity not found");

    Utils::DrawStringStack(ConVars::gsm_margin_right->value, ConVars::gsm_margin_up->value, screenText);
}

void CModeEntityReport::Render3D()
{
    const float colorR = 0.0f;
    const float colorG = 1.0f;
    const float colorB = 0.0f;

    if (m_iEntityIndex <= 0) 
        return;

    Utils::GetEntityBbox(m_iEntityIndex, m_vecBboxMin, m_vecBboxMax);
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

    if (!g_EngineModule.IsSoftwareRenderer())
    {
        g_pClientEngfuncs->pTriAPI->RenderMode(kRenderNormal);
        glDisable(GL_DEPTH_TEST);
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
        glEnable(GL_DEPTH_TEST);
    }
}

int CModeEntityReport::TraceEntity()
{
    vec3_t viewDir;
    vec3_t viewOrigin;
    pmtrace_t traceData;
    const float lineLen = 11590.0f;
    float worldDistance = lineLen;

    m_EntityIndexList.clear();
    m_EntityDistanceList.clear();
    viewOrigin = g_pPlayerMove->origin + g_pPlayerMove->view_ofs;
    viewDir = GetViewDirection();

    Utils::TraceLine(viewOrigin, viewDir, lineLen, &traceData);
    if (traceData.fraction < 1.f)
    {
        if (traceData.ent > 0)
            return g_pClientEngfuncs->pEventAPI->EV_IndexFromTrace(&traceData);
        else
            worldDistance = lineLen * traceData.fraction;
    }

    const int listCount = 3;
    physent_t *physEntLists[listCount] = { g_pPlayerMove->visents, g_pPlayerMove->physents, g_pPlayerMove->moveents };
    int physEntListsLen[listCount] = { g_pPlayerMove->numvisent, g_pPlayerMove->numphysent, g_pPlayerMove->nummoveent };
    for (int i = 0; i < listCount; ++i)
    {
        int physEntIndex = TracePhysEntList(physEntLists[i], physEntListsLen[i], viewOrigin, viewDir, lineLen);
        if (physEntIndex)
        {
            m_EntityIndexList.push_back(physEntIndex);
            m_EntityDistanceList.push_back(GetEntityDistance(physEntIndex));
        }
    }

    // get nearest entity from all lists
    // also add world for comparision
    m_EntityIndexList.push_back(0);
    m_EntityDistanceList.push_back(worldDistance);
    auto &distanceList = m_EntityDistanceList;
    if (distanceList.size() > 1)
    {
        auto iterNearestEnt = std::min_element(std::begin(distanceList), std::end(distanceList));
        if (std::end(distanceList) != iterNearestEnt)
            return m_EntityIndexList[std::distance(distanceList.begin(), iterNearestEnt)];
    }
    return 0;
}

float CModeEntityReport::TracePhysEnt(const physent_t &physEnt, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen)
{
    vec3_t bboxMin;
    vec3_t bboxMax;
    vec3_t lineEnd;

    // skip studiomodel visents which is culled
    Utils::GetEntityBbox(physEnt.info, bboxMin, bboxMax);
    if (!g_pClientEngfuncs->pTriAPI->BoxInPVS(bboxMin, bboxMax))
        return 1.0f;

    // check for intersection
    lineEnd = viewOrigin + (viewDir * lineLen);
    return Utils::TraceBBoxLine(bboxMin, bboxMax, viewOrigin, lineEnd);
}

int CModeEntityReport::TracePhysEntList(physent_t list[], int count, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen)
{
    int entIndex = 0;
    float minFraction = 1.0f;
    
    for (int i = 0; i < count; ++i)
    {
        const physent_t &visEnt = list[i];
        float traceFraction = TracePhysEnt(visEnt, viewOrigin, viewDir, lineLen);
        if (traceFraction < minFraction)
        {
            entIndex    = visEnt.info;
            minFraction = traceFraction;
        }
    }

    return entIndex;
}

float CModeEntityReport::GetEntityDistance(int entityIndex)
{
    vec3_t viewOrigin;
    vec3_t entityOrigin;
    vec3_t pointInBbox;
    vec3_t bboxMin, bboxMax;
    cl_entity_t *traceEntity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
    model_t *entityModel = traceEntity->model;

    viewOrigin = g_pPlayerMove->origin + g_pPlayerMove->view_ofs;
    if (entityModel->type == mod_brush)
        entityOrigin = (entityModel->mins + entityModel->maxs) / 2.f;
    else
        entityOrigin = traceEntity->curstate.origin;

    // get nearest bbox-to-player distance by point caged in bbox
    Utils::GetEntityBbox(entityIndex, bboxMin, bboxMax);
    pointInBbox.x = max(min(viewOrigin.x, bboxMax.x), bboxMin.x);
    pointInBbox.y = max(min(viewOrigin.y, bboxMax.y), bboxMin.y);
    pointInBbox.z = max(min(viewOrigin.z, bboxMax.z), bboxMin.z);
    return (pointInBbox - viewOrigin).Length();
}

vec3_t CModeEntityReport::GetViewDirection()
{
    vec3_t viewAngles, viewDir;
    g_pClientEngfuncs->GetViewAngles(viewAngles);
    g_pClientEngfuncs->pfnAngleVectors(viewAngles, viewDir, nullptr, nullptr);
    return viewDir;
}
