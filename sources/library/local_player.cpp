#include "local_player.h"
#include "utils.h"
#include "client_module.h"
#include <assert.h>

CLocalPlayer &g_LocalPlayer = CLocalPlayer::GetInstance();
CLocalPlayer &CLocalPlayer::GetInstance()
{
    static CLocalPlayer instance;
    return instance;
}

void CLocalPlayer::UpdatePlayerMove(playermove_t *pmove)
{
    m_pPlayerMove = pmove;
}

vec3_t CLocalPlayer::GetOrigin() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->origin;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return localPlayer->origin;
    }
}

vec3_t CLocalPlayer::GetAngles() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->angles;
    }
    else 
    {
        vec3_t angles;
        g_pClientEngfuncs->GetViewAngles(angles);
        return angles;
    }
}

vec3_t CLocalPlayer::GetPunchAngles() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->punchangle;
}

vec3_t CLocalPlayer::GetVelocity() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->velocity;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return Utils::GetEntityVelocityApprox(localPlayer->index);
    }
}

vec3_t CLocalPlayer::GetBaseVelocity() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->basevelocity;
    }
    else {
        return vec3_t(0.f, 0.f, 0.f);
    }
}

vec3_t CLocalPlayer::GetViewOffset() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->view_ofs;
    }
    else {
        vec3_t viewOffset;
        g_pClientEngfuncs->pEventAPI->EV_LocalPlayerViewheight(viewOffset);
        return viewOffset;
    }
}

vec3_t CLocalPlayer::GetViewOrigin() const
{
    return GetOrigin() + GetViewOffset();
}

vec3_t CLocalPlayer::GetViewDirection() const
{
    vec3_t viewAngles, viewDir;
    g_pClientEngfuncs->GetViewAngles(viewAngles);
    g_pClientEngfuncs->pfnAngleVectors(viewAngles, viewDir, nullptr, nullptr);
    return viewDir;
}

float CLocalPlayer::GetMaxSpeed() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->maxspeed;
}

float CLocalPlayer::GetClientMaxSpeed() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->clientmaxspeed;
}

float CLocalPlayer::GetGravity() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->gravity;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return localPlayer->curstate.gravity;
    }
}

float CLocalPlayer::GetFriction() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->friction;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return localPlayer->curstate.friction;
    }
}

float CLocalPlayer::GetDuckTime() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->flDuckTime;
}

bool CLocalPlayer::IsDucking() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->bInDuck;
}

bool CLocalPlayer::OnGround() const
{
    // TODO implement this for case when prediction data unavailable?
    assert(PredictionDataValid());
    return m_pPlayerMove->onground != -1;
}

int CLocalPlayer::GetMovetype() const
{
    if (PredictionDataValid()) {
        return m_pPlayerMove->movetype;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return localPlayer->curstate.movetype;
    }
}

int CLocalPlayer::GetFlags() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->flags;
}

int CLocalPlayer::GetHullType() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->usehull;
}

const physent_t *CLocalPlayer::GetPhysents() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->physents;
}

const physent_t *CLocalPlayer::GetVisents() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->visents;
}

const physent_t *CLocalPlayer::GetMoveents() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->moveents;
}

int CLocalPlayer::GetPhysentsCount() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->numphysent;
}

int CLocalPlayer::GetVisentsCount() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->numvisent;
}

int CLocalPlayer::GetMoveentsCount() const
{
    assert(PredictionDataValid());
    return m_pPlayerMove->nummoveent;
}

int CLocalPlayer::GetIntUserVar(size_t index) const
{
    int *p;
    assert(index >= 1 && index <= 4);
    if (PredictionDataValid()) {
        p = &m_pPlayerMove->iuser1;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        p = &localPlayer->curstate.iuser1;
    }
    return p[index - 1];
}

float CLocalPlayer::GetFloatUserVar(size_t index) const
{
    float *p;
    assert(index >= 1 && index <= 4);
    if (PredictionDataValid()) {
        p = &m_pPlayerMove->fuser1;
    }
    else {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        p = &localPlayer->curstate.fuser1;
    }
    return p[index - 1];
}

bool CLocalPlayer::IsThirdPersonForced() const
{
    bool playerDead = PredictionDataValid() ? m_pPlayerMove->dead : false;
    return ConVars::gsm_thirdperson->value > 0.0f && playerDead;
}

float CLocalPlayer::GetThirdPersonCameraDist() const
{
    pmtrace_t traceInfo;
    vec3_t viewDir = GetViewDirection();
    vec3_t viewOrigin = GetViewOrigin();
    float maxDist = ConVars::gsm_thirdperson_dist->value;
    Utils::TraceLine(viewOrigin, -viewDir, maxDist, &traceInfo);
    return maxDist * traceInfo.fraction;
}

bool CLocalPlayer::PredictionDataValid() const
{
    // we can't use prediction data when demo playing
    bool demoPlaying = g_pClientEngfuncs->pDemoAPI->IsPlayingback() != 0;
    return m_pPlayerMove != nullptr && !demoPlaying;
}

bool CLocalPlayer::IsSpectate() const
{
    // assume that it's valid only for cs 1.6/hl1
    // because other mods can use iuser variables for other purposes
    int specMode, targetIndex;
    if (PredictionDataValid())
    {
        specMode = m_pPlayerMove->iuser1;
        targetIndex = m_pPlayerMove->iuser2;
    }
    else
    {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        specMode = localPlayer->curstate.iuser1;
        targetIndex = localPlayer->curstate.iuser2;
    }
    return specMode != 0 && targetIndex != 0;
}

SpectatingMode CLocalPlayer::GetSpectatingMode() const
{
    if (PredictionDataValid())
    {
        return static_cast<SpectatingMode>(m_pPlayerMove->iuser3);
    }
    else
    {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return static_cast<SpectatingMode>(localPlayer->curstate.iuser3);
    }
}

int CLocalPlayer::GetSpectateTargetIndex() const
{
    if (IsSpectate())
    {
        if (PredictionDataValid()) {
            return m_pPlayerMove->iuser2;
        }
        else 
        {
            cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
            return localPlayer->curstate.iuser2;
        }
    }
    return -1;
}
