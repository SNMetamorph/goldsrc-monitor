#include "local_player.h"
#include "utils.h"
#include "client_module.h"

CLocalPlayer &g_LocalPlayer = CLocalPlayer::GetInstance();
playermove_t *&g_pPlayerMove = g_LocalPlayer.GetPlayerMove();
CLocalPlayer &CLocalPlayer::GetInstance()
{
    static CLocalPlayer instance;
    return instance;
}

void CLocalPlayer::UpdatePlayerMove(playermove_t *pmove)
{
    m_pPlayerMove = pmove;
}

playermove_t* &CLocalPlayer::GetPlayerMove()
{
    return m_pPlayerMove;
}

const vec3_t &CLocalPlayer::GetOrigin() const
{
    return m_pPlayerMove->origin;
}

const vec3_t &CLocalPlayer::GetAngles() const
{
    return m_pPlayerMove->angles;
}

const vec3_t &CLocalPlayer::GetPunchAngles() const
{
    return m_pPlayerMove->punchangle;
}

const vec3_t &CLocalPlayer::GetVelocity() const
{
    return m_pPlayerMove->velocity;
}

const vec3_t &CLocalPlayer::GetBaseVelocity() const
{
    return m_pPlayerMove->basevelocity;
}

float CLocalPlayer::GetVelocityHorz() const
{
    return m_pPlayerMove->velocity.Length2D();
}

const vec3_t &CLocalPlayer::GetViewOffset() const
{
    return m_pPlayerMove->view_ofs;
}

vec3_t CLocalPlayer::GetViewOrigin() const
{
    return m_pPlayerMove->origin + m_pPlayerMove->view_ofs;
}

vec3_t CLocalPlayer::GetViewDirection() const
{
    vec3_t viewAngles, viewDir;
    g_pClientEngfuncs->GetViewAngles(viewAngles);
    g_pClientEngfuncs->pfnAngleVectors(viewAngles, viewDir, nullptr, nullptr);
    return viewDir;
}

bool CLocalPlayer::IsThirdPersonForced() const
{
    return ConVars::gsm_thirdperson->value > 0.0f && !m_pPlayerMove->dead;
}

float CLocalPlayer::GetThirdPersonCameraDist() const
{
    pmtrace_t traceInfo;
    vec3_t viewDir = GetViewDirection();
    vec3_t viewOrigin = m_pPlayerMove->origin + m_pPlayerMove->view_ofs;
    float maxDist = ConVars::gsm_thirdperson_dist->value;
    Utils::TraceLine(viewOrigin, -viewDir, maxDist, &traceInfo);
    return maxDist * traceInfo.fraction;
}

bool CLocalPlayer::PlayerMoveAvailable() const
{
    // we can't use player move when demo playing
    bool demoPlaying = g_pClientEngfuncs->pDemoAPI->IsPlayingback() != 0;
    return m_pPlayerMove != nullptr && !demoPlaying;
}

bool CLocalPlayer::IsSpectate() const
{
    // assume that it's valid only for cs 1.6/hl1
    // because other mods can use iuser variables for other purposes
    int specMode = m_pPlayerMove->iuser1;
    int targetIndex = m_pPlayerMove->iuser2;
    return specMode != 0 && targetIndex != 0;
}

int CLocalPlayer::GetSpectateTargetIndex() const
{
    if (IsSpectate())
    {
        return m_pPlayerMove->iuser2;
    }
    return -1;
}
