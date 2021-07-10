#include "entity_dictionary.h"
#include "utils.h"
#include "client_module.h"

CEntityDictionary &g_EntityDictionary = CEntityDictionary::GetInstance();

CEntityDictionary &CEntityDictionary::GetInstance()
{
    static CEntityDictionary instance;
    return instance;
}

// Should be initialized after every map change
void CEntityDictionary::Initialize()
{
    Reset();
    ParseEntityData();
    FindEntityAssociations();
}

void CEntityDictionary::Reset()
{
    m_EntityDescList.clear();
}

bool CEntityDictionary::FindDescription(int entityIndex, CEntityDescription &destDescription)
{
    vec3_t entityMins;
    vec3_t entityMaxs;
    Utils::GetEntityBbox(entityIndex, entityMins, entityMaxs);
    for (auto it = m_EntityDescList.begin(); it != m_EntityDescList.end(); ++it)
    {
        CEntityDescription &entityDesc = *it;
        if (entityDesc.GetAssocEntityIndex() == entityIndex) {
            destDescription = entityDesc;
            return true;
        }

        const vec3_t &bboxMins = entityDesc.GetBboxMins();
        const vec3_t &bboxMaxs = entityDesc.GetBboxMaxs();
        const vec3_t diffMin = entityMins - bboxMins;
        const vec3_t diffMax = entityMaxs - bboxMaxs;
        if (diffMin.Length() < 1.0f && diffMax.Length() < 1.0f)
        {
            entityDesc.AssociateEntity(entityIndex);
            destDescription = entityDesc;
            return true;
        }
    }
    return false;
}

void CEntityDictionary::ParseEntityData()
{
    cl_entity_t *worldEntity = g_pClientEngfuncs->GetEntityByIndex(0);
    model_t *worldModel = worldEntity->model;
    char *entData = worldModel->entities;
    std::vector<char> key;
    std::vector<char> value;
    std::vector<char> token;
    const int bufferSize = 1024;

    key.resize(bufferSize, '\0');
    value.resize(bufferSize, '\0');
    token.resize(bufferSize, '\0');
    while (true)
    {
        if (!entData || entData[1] == '\0')
            break;

        entData = g_pClientEngfuncs->COM_ParseFile(entData, token.data());
        if (strcmp(token.data(), "{") == 0)
        {
            CEntityDescription entityDesc;
            while (true)
            {
                // unexpected ending of entity data
                if (!entData || entData[0] == '\0')
                    break;

                entData = g_pClientEngfuncs->COM_ParseFile(entData, token.data());
                if (strcmp(token.data(), "}") == 0)
                {
                    entityDesc.Initialize();
                    m_EntityDescList.push_back(entityDesc);
                    break;
                }
                else
                {   
                    if (!entData || entData[0] == '\0')
                        break;

                    strncpy(key.data(), token.data(), sizeof(key) - 1);
                    entData = g_pClientEngfuncs->COM_ParseFile(entData, value.data());
                    entityDesc.AddKeyValue(std::string(key.data()), std::string(value.data()));
                }
            }
        }
    }
}

// Finds associations between entity descriptions and actual game entities
void CEntityDictionary::FindEntityAssociations()
{
    const int entityCount = GetClientMaxEntities();
    const int maxClients = g_pClientEngfuncs->GetMaxClients();
    if (entityCount > 0)
    {
        for (int i = maxClients; i < entityCount; ++i)
        {
            vec3_t entityMins, entityMaxs;
            Utils::GetEntityBbox(i, entityMins, entityMaxs);
            for (auto it = m_EntityDescList.begin(); it != m_EntityDescList.end(); ++it)
            {
                CEntityDescription &entityDesc = *it;
                const vec3_t &bboxMins = entityDesc.GetBboxMins();
                const vec3_t &bboxMaxs = entityDesc.GetBboxMaxs();
                const vec3_t diffMin = entityMins - bboxMins;
                const vec3_t diffMax = entityMaxs - bboxMaxs;
                if (diffMin.Length() < 1.0f && diffMax.Length() < 1.0f)
                {
                    entityDesc.AssociateEntity(i);
                    break;
                }
            }
        }
    }
}

// Very non-optimal, but there is no way to get this directly from engine
int CEntityDictionary::GetClientMaxEntities()
{
    const int iterStep = 75;
    const int maxClients = g_pClientEngfuncs->GetMaxClients();
    for (int i = maxClients; i < 65536; i += iterStep)
    {
        if (!g_pClientEngfuncs->GetEntityByIndex(i)) 
        {
            for (int j = 0; j <= iterStep; ++j)
            {
                if (g_pClientEngfuncs->GetEntityByIndex(i - j)) {
                    return i - j + 1;
                }
            }
        }
    }
    return -1;
}
