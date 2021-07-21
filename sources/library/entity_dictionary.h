#pragma once
#include "entity_description.h"
#include "bvh_tree.h"
#include "hlsdk.h"
#include <vector>

class CEntityDictionary
{
public:
    static CEntityDictionary &GetInstance();
    void Reset();
    void Initialize();
    void VisualizeTree(bool textRendering);
    bool FindDescription(int entityIndex, CEntityDescription &destDescription);

    inline int GetDescriptionsCount() const { return m_EntityDescList.size(); }
    inline bool IsInitialized() const { return m_EntityDescList.size() > 0;  }
    
private:
    CEntityDictionary() {};
    CEntityDictionary(const CEntityDictionary &) = delete;
    CEntityDictionary &operator=(const CEntityDictionary &) = delete;

    void BuildDescriptionsTree();
    void ParseEntityData();
    void FindEntityAssociations();
    int GetClientMaxEntities();

    CBVHTree m_EntityDescTree = CBVHTree(&m_EntityDescList);
    std::vector<CEntityDescription> m_EntityDescList;
};
extern CEntityDictionary &g_EntityDictionary;
