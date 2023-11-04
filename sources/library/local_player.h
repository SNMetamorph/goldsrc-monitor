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
#include "cvars.h"
#include "hlsdk.h"

enum class SpectatingMode 
{
    None,
    ChaseLocked,
    ChaseFree,
    Roaming,    // Free Look
    InEye,      // First Person
    MapFree,    // Free Map Overview
    MapChase,   // Chase Map Overview
};

class CLocalPlayer
{
public:
    CLocalPlayer() {};
    ~CLocalPlayer() = default;
    void UpdatePlayerMove(playermove_t *pmove);
    bool PredictionDataValid() const;

    vec3_t GetOrigin() const;
    vec3_t GetAngles() const;
    vec3_t GetPunchAngles() const;
    vec3_t GetVelocity() const;
    vec3_t GetBaseVelocity() const;
    vec3_t GetViewOffset() const;
    vec3_t GetViewOrigin() const;
    vec3_t GetViewDirection() const;
    float GetMaxSpeed() const;
    float GetClientMaxSpeed() const;
    float GetGravity() const;
    float GetFriction() const;
    float GetDuckTime() const;
    bool IsDucking() const;
    bool OnGround() const;
    int GetMovetype() const;
    int GetFlags() const;
    int GetHullType() const;

    const physent_t *GetPhysents() const;
    const physent_t *GetVisents() const;
    const physent_t *GetMoveents() const;
    int GetPhysentsCount() const;
    int GetVisentsCount() const;
    int GetMoveentsCount() const;

    int GetIntUserVar(size_t index) const;
    float GetFloatUserVar(size_t index) const;

    bool IsSpectate() const;
    SpectatingMode GetSpectatingMode() const;
    int GetSpectateTargetIndex() const;
    bool IsThirdPersonForced() const;
    float GetThirdPersonCameraDist() const;

private:
    playermove_t *m_pPlayerMove = nullptr;
};

