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
    CEntityDictionary();

    void Reset();
    void Initialize();
    void VisualizeTree(bool textRendering);
    void VisualizeDescriptions() const;
    bool FindDescription(int entityIndex, CEntityDescription &destDescription, int &iterCount);

    int GetDescriptionsCount() const { return m_entityDescList.size(); }
    bool IsInitialized() const { return m_entityDescList.size() > 0;  }
    
private:
    void AssociateDescription(int entityIndex, int descIndex);
    void BuildDescriptionsTree();
    void ParseEntityData();

    std::map<int, int> m_associations;
    std::vector<CEntityDescription> m_entityDescList;
    CBVHTree m_entityDescTree;
};
