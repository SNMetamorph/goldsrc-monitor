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
