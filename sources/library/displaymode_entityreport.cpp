/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include "displaymode_entityreport.h"
#include "cvars.h"
#include "utils.h"
#include "client_module.h"
#include "engine_module.h"
#include "studio.h"
#include "local_player.h"
#include "bounding_box.h"
#include <algorithm>
#include <iterator> 

CModeEntityReport::CModeEntityReport(const CLocalPlayer &playerRef, const CEngineModule &moduleRef) 
    : m_localPlayer(playerRef), m_engineModule(moduleRef)
{
    m_entityIndex = 0;
    m_lockedEntityIndex = 0;
    m_entityIndexList = {};
    m_entityDistanceList = {};
}

void CModeEntityReport::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    if (m_localPlayer.PredictionDataValid())
    {
        int debugMode = ConVars::gsm_debug->value;
        if (!m_entityDictionary.IsInitialized())
            m_entityDictionary.Initialize();

        screenText.Clear();
        m_entityIndex = TraceEntity();
        if (!PrintEntityInfo(GetActualEntityIndex(), screenText))
        {
            // disable hull highlighting for this entity
            m_entityIndex = 0;
        }

        if (debugMode == 2) {
            m_entityDictionary.VisualizeTree(true);
        }
    }
    else
    {
        screenText.Clear();
        screenText.Push("This mode unavailable when playing demo");
    }

    Utils::DrawStringStack(
        static_cast<int>(ConVars::gsm_margin_right->value), 
        static_cast<int>(ConVars::gsm_margin_up->value), 
        screenText
    );
}

void CModeEntityReport::Render3D()
{
    CBoundingBox entityBbox;
    int debugMode = ConVars::gsm_debug->value;
    int currentEntity = GetActualEntityIndex();
    const Color colorGreen = Color(0.f, 1.f, 0.f, 1.f);

    if (debugMode == 1) {
        m_entityDictionary.VisualizeDescriptions();
    }
    else if (debugMode == 2) {
        m_entityDictionary.VisualizeTree(false);
    }

    if (currentEntity > 0 && !m_engineModule.IsSoftwareRenderer())
    {
        cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(currentEntity);
        Utils::GetEntityBoundingBox(currentEntity, entityBbox);
        vec3_t centerOffset = (entity->curstate.mins + entity->curstate.maxs) / 2.f;
        Utils::DrawCuboid(entity->origin, centerOffset, entity->angles, entityBbox.GetSize(), colorGreen);
    }
}

bool CModeEntityReport::KeyInput(bool keyDown, int keyCode, const char *bindName)
{
    if (Utils::GetCurrentDisplayMode() != DisplayModeType::EntityReport || !keyDown) {
        return true;
    }

    if (keyCode == 'v')
    {
        if (m_entityIndex > 0) {
            if (m_entityIndex == m_lockedEntityIndex) {
                m_lockedEntityIndex = 0;
            }
            else {
                m_lockedEntityIndex = m_entityIndex;
            }
        }
        else {
            m_lockedEntityIndex = 0;
        }
        g_pClientEngfuncs->pfnPlaySoundByName("buttons/blip1.wav", 0.8f);
        return false;
    }
    return true;
}

void CModeEntityReport::HandleChangelevel()
{
    m_entityDictionary.Reset();
}

int CModeEntityReport::TraceEntity()
{
    vec3_t viewDir;
    vec3_t viewOrigin;
    pmtrace_t traceData;
    const float lineLen = 11590.0f;
    float worldDistance = lineLen;
    int ignoredEnt = -1;

    m_entityIndexList.clear();
    m_entityDistanceList.clear();
    viewOrigin = m_localPlayer.GetViewOrigin();
    viewDir = m_localPlayer.GetViewDirection();

    if (m_localPlayer.IsSpectate())
        ignoredEnt = m_localPlayer.GetSpectateTargetIndex();  

    Utils::TraceLine(viewOrigin, viewDir, lineLen, &traceData, ignoredEnt);
    if (traceData.fraction < 1.f)
    {
        if (traceData.ent > 0)
            return g_pClientEngfuncs->pEventAPI->EV_IndexFromTrace(&traceData);
        else
            worldDistance = lineLen * traceData.fraction;
    }

    const int listCount = 3;
    const physent_t *physEntLists[listCount] = { 
        m_localPlayer.GetVisents(), 
        m_localPlayer.GetPhysents(), 
        m_localPlayer.GetMoveents() 
    };
    int physEntListsLen[listCount] = { 
        m_localPlayer.GetVisentsCount(), 
        m_localPlayer.GetPhysentsCount(), 
        m_localPlayer.GetMoveentsCount() 
    };

    for (int i = 0; i < listCount; ++i)
    {
        int physEntIndex = TracePhysEntList(physEntLists[i], physEntListsLen[i], viewOrigin, viewDir, lineLen);
        if (physEntIndex)
        {
            m_entityIndexList.push_back(physEntIndex);
            m_entityDistanceList.push_back(GetEntityDistance(physEntIndex));
        }
    }

    // get nearest entity from all lists
    // also add world for comparision
    m_entityIndexList.push_back(0);
    m_entityDistanceList.push_back(worldDistance);
    auto &distanceList = m_entityDistanceList;
    if (distanceList.size() > 1)
    {
        auto iterNearestEnt = std::min_element(std::begin(distanceList), std::end(distanceList));
        if (std::end(distanceList) != iterNearestEnt)
            return m_entityIndexList[std::distance(distanceList.begin(), iterNearestEnt)];
    }
    return 0;
}

float CModeEntityReport::TracePhysEnt(const physent_t &physEnt, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen)
{
    CBoundingBox entityBbox;
    Utils::GetEntityBoundingBox(physEnt.info, entityBbox);

    // skip studiomodel visents which is culled
    vec3_t bboxMins = entityBbox.GetMins();
    vec3_t bboxMaxs = entityBbox.GetMaxs();
    if (!g_pClientEngfuncs->pTriAPI->BoxInPVS(bboxMins, bboxMaxs)) {
        return 1.0f;
    }

    // check for intersection
    vec3_t lineEnd = viewOrigin + (viewDir * lineLen);
    return Utils::TraceBBoxLine(entityBbox, viewOrigin, lineEnd);
}

int CModeEntityReport::TracePhysEntList(const physent_t *list, int count, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen)
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
    vec3_t pointInBbox;
    CBoundingBox entityBbox;
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
    if (entity)
    {
        model_t *entityModel = entity->model;
        vec3_t viewOrigin = m_localPlayer.GetViewOrigin();

        // get nearest bbox-to-player distance by point caged in bbox
        Utils::GetEntityBoundingBox(entityIndex, entityBbox);
        const vec3_t &bboxMins = entityBbox.GetMins();
        const vec3_t &bboxMaxs = entityBbox.GetMaxs();
        pointInBbox.x = std::max(std::min(viewOrigin.x, bboxMaxs.x), bboxMins.x);
        pointInBbox.y = std::max(std::min(viewOrigin.y, bboxMaxs.y), bboxMins.y);
        pointInBbox.z = std::max(std::min(viewOrigin.z, bboxMaxs.z), bboxMins.z);
        return (pointInBbox - viewOrigin).Length();
    }
    return 0.0f;
}

bool CModeEntityReport::PrintEntityInfo(int entityIndex, CStringStack &screenText)
{
    int debugMode = ConVars::gsm_debug->value;
    if (entityIndex < 1)
    {
        std::string mapName = Utils::GetCurrentMapName();
        screenText.Push("Entity not found");
        screenText.PushPrintf("Map: %s", mapName.c_str());
        screenText.PushPrintf("Entity descriptions: %d", m_entityDictionary.GetDescriptionsCount());
    }
    else if (Utils::IsGameDirEquals("cstrike") && m_localPlayer.IsSpectate() && m_localPlayer.GetSpectatingMode() != SpectatingMode::Roaming)
    {
        // disable print in non free-look spectating modes
        screenText.Push("Print enabled only in free look mode");
        return false;
    }
    else
    {
        int iterCount;
        CEntityDescription entityDesc;
        cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
        bool descFound = m_entityDictionary.FindDescription(entityIndex, entityDesc, iterCount);

        PrintEntityCommonInfo(entityIndex, screenText);
        if (descFound)
        {
            const std::string &classname = entityDesc.GetClassname();
            const std::string &targetname = entityDesc.GetTargetname();
            screenText.PushPrintf("Classname: %s", classname.c_str());
            if (targetname.length() > 0)
                screenText.PushPrintf("Targetname: %s", targetname.c_str());
        }

        if (entity && entity->model && entity->model->type == mod_studio)
        {
            std::string modelName;
            Utils::GetEntityModelName(entityIndex, modelName);
            screenText.PushPrintf("Model Name: %s", modelName.c_str());
            screenText.PushPrintf("Anim. Frame: %.1f", entity->curstate.frame);
            screenText.PushPrintf("Anim. Sequence: %d", entity->curstate.sequence);
            screenText.PushPrintf("Bodygroup Number: %d", entity->curstate.body);
            screenText.PushPrintf("Skin Number: %d", entity->curstate.skin);
        }

        if (descFound)
        {
            if (debugMode == 2) {
                screenText.PushPrintf("Search iteration count: %d", iterCount);
            }

            const int propsCount = entityDesc.GetPropertiesCount();
            if (propsCount > 0)
            {
                std::string propsString;
                screenText.Push("Entity Properties");
                for (int i = 0; i < propsCount; ++i)
                {
                    entityDesc.GetPropertyString(i, propsString);
                    screenText.PushPrintf("    %s", propsString.c_str());
                }
            }
            else {
                screenText.Push("No entity properties");
            }
        }
        else {
            screenText.Push("Entity properties not found");
        }

        if (!m_lockedEntityIndex) {
            screenText.Push("Press V to hold on current entity");
        }
    }
    return true;
}

void CModeEntityReport::PrintEntityCommonInfo(int entityIndex, CStringStack &screenText)
{
    CBoundingBox entityBbox;
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
    if (entity)
    {
        vec3_t entityVelocity = Utils::GetEntityVelocityApprox(entityIndex);
        vec3_t centerOffset = (entity->curstate.mins + entity->curstate.maxs) / 2.f;
        vec3_t entityAngles = entity->curstate.angles;
        vec3_t entityOrigin = entity->origin + centerOffset;
        Utils::GetEntityBoundingBox(entityIndex, entityBbox);

        screenText.PushPrintf("Entity Index: %d", entityIndex);
        screenText.PushPrintf("Origin: (%.1f; %.1f; %.1f)",
            entityOrigin.x, entityOrigin.y, entityOrigin.z);
        screenText.PushPrintf("Distance: %.1f units",
            GetEntityDistance(entityIndex));
        screenText.PushPrintf("Velocity: %.2f u/s (%.1f; %.1f; %.1f)",
            entityVelocity.Length2D(), entityVelocity.x, entityVelocity.y, entityVelocity.z);
        screenText.PushPrintf("Angles: (%.1f; %.1f; %.1f)",
            entityAngles.x, entityAngles.y, entityAngles.z);
        screenText.PushPrintf("Hull Size: (%.1f; %.1f; %.1f)",
            entityBbox.GetSize().x, entityBbox.GetSize().y, entityBbox.GetSize().z);
        screenText.PushPrintf("Movetype: %s", Utils::GetMovetypeName(entity->curstate.movetype));
        screenText.PushPrintf("Render Mode: %s", Utils::GetRenderModeName(entity->curstate.rendermode));
        screenText.PushPrintf("Render FX: %s", Utils::GetRenderFxName(entity->curstate.renderfx));
        screenText.PushPrintf("Render Amount: %d", entity->curstate.renderamt);
        screenText.PushPrintf("Render Color: %d %d %d",
            entity->curstate.rendercolor.r,
            entity->curstate.rendercolor.g,
            entity->curstate.rendercolor.b
        );
    }
}

int CModeEntityReport::GetActualEntityIndex()
{
    if (m_lockedEntityIndex > 0) {
        return m_lockedEntityIndex;
    }
    else {
        return m_entityIndex;
    }
}
