#pragma once
#include "hlsdk.h"
#include "bounding_box.h"
#include <string>
#include <map>

class CEntityDescription
{
public:
    CEntityDescription();
    void AddKeyValue(const std::string &key, const std::string &value);
    void Initialize();
    void GetPropertyString(int index, std::string &buffer) const;
    void AssociateEntity(int entityIndex);

    inline int GetPropertiesCount() const { return m_EntityProps.size(); }
    inline int GetAssocEntityIndex() const { return m_iAssociatedEntity; }
    inline const std::string& GetClassname() const  { return m_szClassname; }
    inline const std::string& GetTargetname() const { return m_szTargetname; }
    inline const std::string& GetModelName() const  { return m_szModelName; }
    inline const vec3_t &GetOrigin() const { return m_vecOrigin; }
    inline const vec3_t &GetAngles() const { return m_vecAngles; }
    inline const CBoundingBox &GetBoundingBox() const { return m_BoundingBox; }

private:
    void Reset();
    void EstimateBoundingBox();
    void ParseEntityData();
    model_t *FindModelByName(const char *name);

    std::string m_szClassname;
    std::string m_szTargetname;
    std::string m_szModelName;
    vec3_t m_vecAngles;
    vec3_t m_vecOrigin;
    int m_iAssociatedEntity;
    CBoundingBox m_BoundingBox;
    std::map<std::string, std::string> m_EntityProps;
};
