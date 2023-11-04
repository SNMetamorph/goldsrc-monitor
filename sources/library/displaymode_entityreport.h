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
#include "stdafx.h"
#include "display_mode.h"
#include "engine_module.h"
#include "local_player.h"
#include "entity_dictionary.h"
#include <vector>

class CModeEntityReport : public IDisplayMode
{
public: 
    CModeEntityReport(const CLocalPlayer &playerRef, const CEngineModule &moduleRef);
    virtual ~CModeEntityReport() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override;
    bool KeyInput(bool keyDown, int keyCode, const char *bindName) override;
    void HandleChangelevel() override;
    DisplayModeType GetModeIndex() override { return DisplayModeType::EntityReport; };

private:
    int TraceEntity();
    float TracePhysEnt(const physent_t &physEnt, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);
    int TracePhysEntList(const physent_t *list, int count, vec3_t &viewOrigin, vec3_t &viewDir, float lineLen);
    float GetEntityDistance(int entityIndex);
    bool PrintEntityInfo(int entityIndex, CStringStack &screenText);
    void PrintEntityCommonInfo(int entityIndex, CStringStack &screenText);
    int GetActualEntityIndex();

    int m_entityIndex;
    int m_lockedEntityIndex;
    std::vector<int> m_entityIndexList;
    std::vector<float> m_entityDistanceList;
    CEntityDictionary m_entityDictionary;
    const CLocalPlayer &m_localPlayer;
    const CEngineModule &m_engineModule;
};
