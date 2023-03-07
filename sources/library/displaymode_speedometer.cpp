#include "displaymode_speedometer.h"
#include "client_module.h"
#include "utils.h"
#include "local_player.h"

void CModeSpeedometer::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    const int centerX = scrWidth / 2;
    const int centerY = scrHeight / 2;
    const int speedometerMargin = 35;
    const float speedUpdateInterval = 0.125f;

    float currentTime = Utils::GetCurrentSysTime();
    float updateTimeDelta = currentTime - m_flLastUpdateTime;
    if (updateTimeDelta >= speedUpdateInterval) 
    {
        CalculateVelocity(frameTime);
        m_flLastUpdateTime = currentTime;
    }

    //DrawVelocityBar(centerX, centerY, m_flVelocity);
    screenText.Clear();
    screenText.PushPrintf("%3.1f", m_flVelocity);

    int stringWidth = Utils::GetStringWidth(screenText.StringAt(0));
    Utils::DrawStringStack(
        centerX + stringWidth / 2,
        centerY + speedometerMargin,
        screenText
    );
}

void CModeSpeedometer::DrawVelocityBar(int centerX, int centerY, float velocity) const
{
    const int barHeight = 15;
    const int barMargin = 60;
    const int barWidth = 100.f / 600.f * velocity;
    g_pClientEngfuncs->pfnFillRGBA(
        centerX - (barWidth / 2),
        centerY + barMargin,
        barWidth,
        barHeight,
        0, 255, 0, 200
    );
}

void CModeSpeedometer::CalculateVelocity(float frameTime)
{
    if (LocalPlayerSpectating()) {
        m_flVelocity = GetEntityVelocityApprox(GetSpectatedTargetIndex());
    }
    else {
        m_flVelocity = GetLocalPlayerVelocity();
    }
}

float CModeSpeedometer::GetEntityVelocityApprox(int entityIndex) const
{           
    if (g_pClientEngfuncs->GetEntityByIndex(entityIndex)) {
        return Utils::GetEntityVelocityApprox(entityIndex).Length2D();
    }
    return 0.0f;
}

int CModeSpeedometer::GetSpectatedTargetIndex() const
{
    if (g_LocalPlayer.PlayerMoveAvailable()) {
        return g_LocalPlayer.GetSpectateTargetIndex();
    }
    else 
    {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return localPlayer->curstate.iuser2;
    }
}

bool CModeSpeedometer::LocalPlayerSpectating() const
{
    if (g_LocalPlayer.PlayerMoveAvailable()) {
        return g_LocalPlayer.IsSpectate();
    }
    else 
    {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        int specMode = localPlayer->curstate.iuser1;
        int targetIndex = localPlayer->curstate.iuser2;
        return specMode != 0 && targetIndex != 0;
    }
}

float CModeSpeedometer::GetLocalPlayerVelocity() const
{
    if (g_LocalPlayer.PlayerMoveAvailable()) {
        return (g_pPlayerMove->velocity + g_pPlayerMove->basevelocity).Length2D();
    }
    else 
    {
        cl_entity_t *localPlayer = g_pClientEngfuncs->GetLocalPlayer();
        return Utils::GetEntityVelocityApprox(localPlayer->index).Length2D();
    }
}
