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
    BuildDescriptionsTree();
}

void CEntityDictionary::VisualizeTree(bool textRendering)
{
    m_EntityDescTree.Visualize(textRendering);
}

void CEntityDictionary::VisualizeDescriptions() const
{
    for (int i = 0; i < m_EntityDescList.size(); ++i)
    {
        const CEntityDescription &description = m_EntityDescList[i];
        Utils::DrawCuboid(
            description.GetOrigin(), 
            vec3_t(0, 0, 0), 
            description.GetAngles(), 
            description.GetBoundingBox().GetSize(), 
            Color::GetRandom(i)
        );
    }
}

void CEntityDictionary::Reset()
{
    m_EntityDescList.clear();
    m_Associations.clear();
}

bool CEntityDictionary::FindDescription(int entityIndex, CEntityDescription &destDescription, int &iterCount)
{
    int nodeIndex;
    CBoundingBox entityBoundingBox;
    Utils::GetEntityBoundingBox(entityIndex, entityBoundingBox);
    if (m_Associations.count(entityIndex) > 0)
    {
        int descIndex = m_Associations[entityIndex];
        destDescription = m_EntityDescList[descIndex];
        return true;
    }
    else if (m_EntityDescTree.FindLeaf(entityBoundingBox, nodeIndex, iterCount))
    {
        const CBVHTreeNode &node = m_EntityDescTree.GetNode(nodeIndex);
        int descIndex = node.GetDescriptionIndex();
        AssociateDescription(entityIndex, descIndex);
        destDescription = m_EntityDescList[descIndex];
        return true;
    }
    return false;
}

void CEntityDictionary::AssociateDescription(int entityIndex, int descIndex)
{
    CEntityDescription &entityDesc = m_EntityDescList[descIndex];
    entityDesc.AssociateEntity(entityIndex);
    m_Associations.insert({ entityIndex, descIndex });
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
    const int bufferSize = 2048;

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
