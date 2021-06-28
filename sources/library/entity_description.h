#pragma once
#include "hlsdk.h"
#include <string>
#include <map>

class CEntityDescription
{
public:
    CEntityDescription();
    void AddKeyValue(const std::string &key, const std::string &value);
    void Initialize();
    void GetPropertiesString(int index, std::string &buffer) const;

    inline int GetPropertiesCount() const { return m_EntityProps.size(); };
    inline const std::string& GetClassname() const  { return m_szClassname; }
    inline const std::string& GetTargetname() const { return m_szTargetname; }
    inline const std::string& GetModelName() const  { return m_szModelName; }
    inline const vec3_t &GetOrigin() const { return m_vecOrigin; }
    inline const vec3_t &GetAngles() const { return m_vecAngles; }
    inline const vec3_t &GetBboxMins() const { return m_vecBboxMins; }
    inline const vec3_t &GetBboxMaxs() const { return m_vecBboxMaxs; }

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
    vec3_t m_vecBboxMins;
    vec3_t m_vecBboxMaxs;
    std::map<std::string, std::string> m_EntityProps;
};
