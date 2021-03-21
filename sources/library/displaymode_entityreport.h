#pragma once
#include "stdafx.h"
#include "display_mode.h"
#include <vector>

class CModeEntityReport : public IDisplayMode
{
public: 
    CModeEntityReport() {};
    virtual ~CModeEntityReport() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override;
    bool KeyInput(int, int, const char *) override { return true; };
    void HandleChangelevel() override;

private:
    int TraceEntity();
    float TracePhysEnt(const physent_t &physEnt, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);
    int TracePhysEntList(physent_t list[], int count, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);
    float GetEntityDistance(int entityIndex);
    
    int m_iEntityIndex;
    std::vector<int> m_EntityIndexList;
    std::vector<float> m_EntityDistanceList;
    vec3_t m_vecBboxMin;
    vec3_t m_vecBboxMax;
};
