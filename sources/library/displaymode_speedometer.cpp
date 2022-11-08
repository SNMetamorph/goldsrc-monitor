#include "displaymode_speedometer.h"
#include "client_module.h"
#include "utils.h"
#include "local_player.h"

void CModeSpeedometer::Render2D(float frameTime, int scrWidth, int scrHeight, CStringStack &screenText)
{
    int stringWidth;
    const int centerX = scrWidth / 2;
    const int centerY = scrHeight / 2;
    const int speedometerMargin = 35;
    float velocity;

    screenText.Clear();
    if (g_LocalPlayer.IsSpectate())
    {
        int targetIndex = g_LocalPlayer.GetSpectateTargetIndex();
        if (g_pClientEngfuncs->GetEntityByIndex(targetIndex) != nullptr)
        {
            velocity = Utils::GetEntityVelocityApprox(targetIndex).Length2D(); 
            //DrawVelocityBar(centerX, centerY, velocity);
        }
        else
            return;
    }
    else
        velocity = g_LocalPlayer.GetVelocityHorz();

    screenText.PushPrintf("%3.2f", velocity);
    stringWidth = Utils::GetStringWidth(screenText.StringAt(0));
    Utils::DrawStringStack(
        centerX + stringWidth / 2,
        centerY + speedometerMargin,
        screenText
    );
}

void CModeSpeedometer::DrawVelocityBar(int centerX, int centerY, float velocity)
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
