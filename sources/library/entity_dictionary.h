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

#pragma once
#include "entity_description.h"
#include "bvh_tree.h"
#include "hlsdk.h"
#include <vector>
#include <map>

class CEntityDictionary
{
public:
    static CEntityDictionary &GetInstance();
    void Reset();
    void Initialize();
    void VisualizeTree(bool textRendering);
    void VisualizeDescriptions() const;
    bool FindDescription(int entityIndex, CEntityDescription &destDescription, int &iterCount);

    inline int GetDescriptionsCount() const { return m_EntityDescList.size(); }
    inline bool IsInitialized() const { return m_EntityDescList.size() > 0;  }
    
private:
    CEntityDictionary() {};
    CEntityDictionary(const CEntityDictionary &) = delete;
    CEntityDictionary &operator=(const CEntityDictionary &) = delete;

    void AssociateDescription(int entityIndex, int descIndex);
    void BuildDescriptionsTree();
    void ParseEntityData();

    std::map<int, int> m_Associations;
    CBVHTree m_EntityDescTree = CBVHTree(&m_EntityDescList);
    std::vector<CEntityDescription> m_EntityDescList;
};
extern CEntityDictionary &g_EntityDictionary;
