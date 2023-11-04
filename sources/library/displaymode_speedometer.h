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
#include "hlsdk.h"

class CModeSpeedometer : public IDisplayMode
{
public: 
    CModeSpeedometer() {};
    virtual ~CModeSpeedometer() {};

    void Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText) override;
    void Render3D() override {};
    bool KeyInput(bool, int, const char *) override { return true; };
    void HandleChangelevel() override {};
    DisplayModeType GetModeIndex() override { return DisplayModeType::Speedometer; };

private:
    void CalculateVelocity(float frameTime);
    float GetEntityVelocityApprox(int entityIndex) const;
    void DrawVelocityBar(int scrWidth, int scrHeight, float velocity) const;

    float m_velocity;
    float m_lastUpdateTime;
};
