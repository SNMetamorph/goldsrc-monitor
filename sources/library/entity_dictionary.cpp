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
    BuildDescriptionsTree();
}

void CEntityDictionary::VisualizeTree(bool textRendering)
{
    m_EntityDescTree.Visualize(textRendering);
}

void CEntityDictionary::Reset()
{
    m_EntityDescList.clear();
}

bool CEntityDictionary::FindDescription(int entityIndex, CEntityDescription &destDescription)
{
    int nodeIndex;
    CBoundingBox entityBoundingBox;
    Utils::GetEntityBoundingBox(entityIndex, entityBoundingBox);
    if (m_EntityDescTree.FindLeaf(entityBoundingBox, nodeIndex)) 
    {
        const CBVHTreeNode &node = m_EntityDescTree.GetNode(nodeIndex);
        destDescription = m_EntityDescList[node.GetDescriptionIndex()];
        return true;
    }
    return false;
}

void CEntityDictionary::BuildDescriptionsTree()
{
    m_EntityDescTree.Reset();
    m_EntityDescTree.Build();
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
            CBoundingBox entityBoundingBox;
            Utils::GetEntityBoundingBox(i, entityBoundingBox);
            for (auto it = m_EntityDescList.begin(); it != m_EntityDescList.end(); ++it)
            {
                CEntityDescription &entityDesc = *it;
                const CBoundingBox &descBoundingBox = entityDesc.GetBoundingBox();
                const vec3_t diffMin = entityBoundingBox.GetMins() - descBoundingBox.GetMins();
                const vec3_t diffMax = entityBoundingBox.GetMaxs() - descBoundingBox.GetMaxs();
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
    int leftBound = 0;
    int rightBound = 65536;
    const int maxClients = g_pClientEngfuncs->GetMaxClients();
    while (true)
    {
        int center = (leftBound + rightBound) / 2;
        if (g_pClientEngfuncs->GetEntityByIndex(center))
            leftBound = center;
        else
            rightBound = center;

        if (abs(leftBound - rightBound) < 2)
        {
            return rightBound;
        }
    }
    return -1;
}
