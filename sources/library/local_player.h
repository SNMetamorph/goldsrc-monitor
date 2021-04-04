#pragma once
#include "cvars.h"
#include "hlsdk.h"

class CLocalPlayer
{
public:
    static CLocalPlayer &GetInstance();
    void Setup(playermove_t *pmove);
    playermove_t* &GetPlayerMove();

    const vec3_t &GetOrigin() const;
    const vec3_t &GetAngles() const;
    const vec3_t &GetPunchAngles() const;
    const vec3_t &GetVelocity() const;
    const vec3_t &GetBaseVelocity() const;
    float GetVelocityHorz() const;
    const vec3_t &GetViewOffset() const;
    vec3_t GetViewOrigin() const;
    vec3_t GetViewDirection() const;

    bool IsSpectate() const;
    int GetSpectateTargetIndex() const;
    bool IsThirdPersonForced() const;
    float GetThirdPersonCameraDist() const;
private:
    CLocalPlayer() : m_pPlayerMove(nullptr) {};
    ~CLocalPlayer() {};

    playermove_t *m_pPlayerMove;
};
extern playermove_t* &g_pPlayerMove;
extern CLocalPlayer &g_LocalPlayer;
