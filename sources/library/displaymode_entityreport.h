#pragma once
#include "stdafx.h"
#include "display_mode.h"
#include <vector>

class CModeEntityReport : public IDisplayMode
{
public: 
    CModeEntityReport();
    virtual ~CModeEntityReport() {};

    void Render2D(int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override;
    bool KeyInput(bool keyDown, int keyCode, const char *bindName) override;
    void HandleChangelevel() override;
    DisplayModeIndex GetModeIndex() override { return DISPLAYMODE_ENTITYREPORT; };

private:
    int TraceEntity();
    float TracePhysEnt(const physent_t &physEnt, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);
    int TracePhysEntList(physent_t list[], int count, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);
    float GetEntityDistance(int entityIndex);
    bool PrintEntityInfo(int entityIndex, CStringStack &screenText);
    void PrintEntityCommonInfo(int entityIndex, CStringStack &screenText);
    int GetActualEntityIndex();

    int m_iEntityIndex;
    int m_iLockedEntityIndex;
    std::vector<int> m_EntityIndexList;
    std::vector<float> m_EntityDistanceList;
};
