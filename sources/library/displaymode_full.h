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
#include "display_mode.h"
#include "local_player.h"

class CModeFull : public IDisplayMode
{
public: 
    CModeFull(const CLocalPlayer &playerRef);
    virtual ~CModeFull() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(bool, int, const char *) override { return true; };
    void HandleChangelevel() override {};
    DisplayModeType GetModeIndex() override { return DisplayModeType::Full; };

private:
    float GetSmoothSystemFrametime();

    float m_frameTime;
    float m_lastFrameTime;
    float m_lastSysTime;
    const CLocalPlayer &m_localPlayer;
};
