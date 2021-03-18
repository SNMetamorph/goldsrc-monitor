#include "entity_description.h"
#include "globals.h"
#include "studio.h"
#include <sstream>

CEntityDescription::CEntityDescription()
{
    Reset();
}

void CEntityDescription::AddKeyValue(const std::string &key, const std::string &value)
{
    m_EntityData.insert({ key, value });
}

void CEntityDescription::Initialize()
{
    ParseEntityData();
    RecognizeBbox();
}

void CEntityDescription::GetKeyValueString(int index, std::string &buffer) const
{
    int currIndex = 0;
    buffer.clear();
    buffer.reserve(256);
    for (auto it = m_EntityData.begin(); it != m_EntityData.end(); ++it)
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

void CEntityDescription::Reset()
{
    const vec3_t vecNull = vec3_t(0, 0, 0);
    m_vecOrigin = vecNull;
    m_vecAngles = vecNull;
    m_vecBboxMins = vecNull;
    m_vecBboxMaxs = vecNull;
}

void CEntityDescription::RecognizeBbox()
{
    cl_entity_t *worldEntity = g_pClientEngFuncs->GetEntityByIndex(0);
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
            m_vecBboxMins = submodelMins;
            m_vecBboxMaxs = submodelMaxs;
            m_vecOrigin = submodelMaxs - hullSize / 2;
        }
    }
    else if (m_vecOrigin.Length() > 0.01f) // is origin != (0, 0, 0)
    {
        if (m_szModelName.length() < 1 || m_szModelName.find(".mdl") == std::string::npos) // point entity
        {
            const vec3_t pointEntityHull = vec3_t(32, 32, 32) / 2;
            m_vecBboxMins = m_vecOrigin - pointEntityHull;
            m_vecBboxMaxs = m_vecOrigin + pointEntityHull;
        }
        else // studio model entity
        {
            model_t *model = FindModelByName(m_szModelName.c_str());
            if (model && model->type == mod_studio)
            {
                studiohdr_t *mdlHeader = (studiohdr_t *)model->cache.data;
                mstudioseqdesc_t *seqDesc = (mstudioseqdesc_t *)((char *)mdlHeader + mdlHeader->seqindex);
                m_vecBboxMins = m_vecOrigin + seqDesc[0].bbmin;
                m_vecBboxMaxs = m_vecOrigin + seqDesc[0].bbmax;
            }
        }
    }
}

void CEntityDescription::ParseEntityData()
{
    for (auto it = m_EntityData.begin(); it != m_EntityData.end(); ++it)
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
            std::istringstream origin_str(value);
            origin_str >> m_vecOrigin.x >> m_vecOrigin.y >> m_vecOrigin.z;
        }
        else if (key.compare("angles") == 0)
        {
            std::istringstream angles_str(value);
            angles_str >> m_vecAngles.x >> m_vecAngles.y >> m_vecAngles.z;
        }
    }
}

model_t *CEntityDescription::FindModelByName(const char *name)
{
    const int maxModelCount = 512; // fixed for GoldSrc
    model_t *modelList = g_pClientEngFuncs->hudGetModelByIndex(1);

    if (strlen(name) < 1)
        return nullptr;

    for (int i = 0; i < maxModelCount; ++i)
    {
        model_t *currModel = &modelList[i];
        if (currModel)
        {
            if (strncmp(currModel->name, name, MAX_MODEL_NAME) == 0)
                return currModel;
        }
    }
    return nullptr;
}
