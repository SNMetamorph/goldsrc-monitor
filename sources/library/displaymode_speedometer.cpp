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
    if (g_LocalPlayer.IsSpectate()) {
        m_flVelocity = GetEntityVelocityApprox(g_LocalPlayer.GetSpectateTargetIndex());
    }
    else {
        m_flVelocity = (g_LocalPlayer.GetVelocity() + g_LocalPlayer.GetBaseVelocity()).Length2D();
    }
}

float CModeSpeedometer::GetEntityVelocityApprox(int entityIndex) const
{           
    if (g_pClientEngfuncs->GetEntityByIndex(entityIndex)) {
        return Utils::GetEntityVelocityApprox(entityIndex).Length2D();
    }
    return 0.0f;
}
