#pragma once
#include "entity_description.h"
#include "hlsdk.h"
#include <vector>

class CEntityDictionary
{
public:
    static CEntityDictionary &GetInstance();
    void Initialize();
    void Reset();
    inline bool IsInitialized() const { return m_isInitialized;  }
    bool FindDescription(int entityIndex, CEntityDescription &destDescription) const;

private:
    CEntityDictionary() {};
    CEntityDictionary(const CEntityDictionary &) = delete;
    CEntityDictionary &operator=(const CEntityDictionary &) = delete;

    void ParseEntityData();
    void InitDescriptions();

    int m_iParsedEntityCount = 0;
    bool m_isInitialized = false;
    std::vector<CEntityDescription> m_EntityDescList;
};
extern CEntityDictionary &g_EntityDictionary;
