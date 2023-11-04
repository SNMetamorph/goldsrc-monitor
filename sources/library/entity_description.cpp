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

#include "entity_description.h"
#include "client_module.h"
#include "engine_module.h"
#include "studio.h"
#include <sstream>

CEntityDescription::CEntityDescription()
{
    Reset();
}

void CEntityDescription::AddKeyValue(const std::string &key, const std::string &value)
{
    m_EntityProps.insert({ key, value });
}

void CEntityDescription::Initialize()
{
    ParseEntityData();
    EstimateBoundingBox();
}

void CEntityDescription::GetPropertyString(int index, std::string &buffer) const
{
    int currIndex = 0;
    buffer.clear();
    buffer.reserve(256);
    for (auto it = m_EntityProps.begin(); it != m_EntityProps.end(); ++it)
    {
        const std::string &key = it->first;
        const std::string &value = it->second;
        if (currIndex == index)
        {
            buffer.append(key + " : " + value);
            return;
        }
        ++currIndex;
    }
}

void CEntityDescription::AssociateEntity(int entityIndex)
{
    m_iAssociatedEntity = entityIndex;
}

void CEntityDescription::Reset()
{
    const vec3_t vecNull = vec3_t(0, 0, 0);
    m_vecOrigin = vecNull;
    m_vecAngles = vecNull;
    m_szClassname.clear();
    m_szTargetname.clear();
    m_szModelName.clear();
    m_boundingBox = CBoundingBox(vecNull);
    m_iAssociatedEntity = -1;
}

void CEntityDescription::EstimateBoundingBox()
{
    cl_entity_t *worldEntity = g_pClientEngfuncs->GetEntityByIndex(0);
    model_t *worldModel = worldEntity->model;
    if (m_szModelName[0] == '*') // brush model
    {
        int submodelIndex = std::atoi(&m_szModelName[1]);
        if (submodelIndex < worldModel->numsubmodels)
        {
            dmodel_t *submodel = &worldModel->submodels[submodelIndex];
            vec3_t submodelMaxs = submodel->maxs;
            vec3_t submodelMins = submodel->mins;
            vec3_t hullSize = submodelMaxs - submodelMins;
            m_boundingBox = CBoundingBox(submodelMins, submodelMaxs);
            m_vecOrigin = m_boundingBox.GetCenterPoint();
        }
    }
    else if (m_vecOrigin.Length() > 0.01f) // is origin != (0, 0, 0)
    {
        // studio model entity
        if (m_szModelName.length() >= 1 && m_szModelName.find(".mdl") != std::string::npos)
        {
            model_t *model = FindModelByName(m_szModelName.c_str());
            if (model && model->type == mod_studio)
            {
                studiohdr_t *mdlHeader = (studiohdr_t *)model->cache.data;
                if (mdlHeader)
                {
                    mstudioseqdesc_t *seqDesc = (mstudioseqdesc_t *)((char *)mdlHeader + mdlHeader->seqindex);
                    m_boundingBox = CBoundingBox(seqDesc[0].bbmin, seqDesc[0].bbmax);
                    m_boundingBox.SetCenterToPoint(m_vecOrigin);
                    return;
                }
            }
        }
        
        // otherwise just use fixed-size hull for point entities
        const vec3_t pointEntityHull = vec3_t(32, 32, 32);
        m_boundingBox = CBoundingBox(pointEntityHull);
        m_boundingBox.SetCenterToPoint(m_vecOrigin);
    }
}

void CEntityDescription::ParseEntityData()
{
    std::istringstream tokenStream;
    for (auto it = m_EntityProps.begin(); it != m_EntityProps.end();)
    {
        const std::string &key = it->first;
        const std::string &value = it->second;

        if (key.compare("classname") == 0)
            m_szClassname.assign(value);
        else if (key.compare("targetname") == 0)
            m_szTargetname.assign(value);
        else if (key.compare("model") == 0)
            m_szModelName.assign(value);
        else if (key.compare("origin") == 0)
        {
            tokenStream.str(value);
            tokenStream >> m_vecOrigin.x >> m_vecOrigin.y >> m_vecOrigin.z;
        }
        else if (key.compare("angles") == 0)
        {
            tokenStream.str(value);
            tokenStream >> m_vecAngles.x >> m_vecAngles.y >> m_vecAngles.z;
        }
        else 
        {
            // check if we should remove property
            if (key.compare("renderamt") != 0 && 
                key.compare("rendercolor") != 0 && 
                key.compare("rendermode") != 0)
            {
                ++it;
                continue; // keep unhandled properties
            }
        };
        // erase properties which already parsed
        it = m_EntityProps.erase(it);
    }
}

model_t *CEntityDescription::FindModelByName(const char *name)
{
    int modelIndex = g_pClientEngfuncs->pEventAPI->EV_FindModelIndex(name);
    if (modelIndex > 0) {
        return g_pClientEngfuncs->hudGetModelByIndex(modelIndex);
    }
    return nullptr;
}
