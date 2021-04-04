#include "displaymode_speedometer.h"
#include "client_module.h"
#include "utils.h"
#include "local_player.h"

void CModeSpeedometer::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    int stringWidth;
    const int centerX = scrWidth / 2;
    const int centerY = scrHeight / 2;
    const int speedometerMargin = 35;
    const int stateOffset = 22; // higher - more smooth velocity
    float velocity;

    screenText.Clear();
    if (g_LocalPlayer.IsSpectate())
    {
        int targetIndex = g_LocalPlayer.GetSpectateTargetIndex();
        cl_entity_t *targetEnt = g_pClientEngfuncs->GetEntityByIndex(targetIndex);
        if (targetEnt)
        {
            const int currIndex = targetEnt->current_position;
            position_history_t &currState = targetEnt->ph[currIndex & HISTORY_MASK];
            position_history_t &prevState = targetEnt->ph[(currIndex - stateOffset) & HISTORY_MASK];
            float timeDelta = currState.animtime - prevState.animtime;
            vec3_t originDelta = currState.origin - prevState.origin;
            velocity = (originDelta / timeDelta).Length2D(); 
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
