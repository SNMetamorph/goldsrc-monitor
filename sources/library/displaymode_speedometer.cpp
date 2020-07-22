#include "displaymode_speedometer.h"
#include "core.h"
#include "globals.h"

CModeSpeedometer &g_ModeSpeedometer = CModeSpeedometer::GetInstance();

CModeSpeedometer &CModeSpeedometer::GetInstance()
{
    static CModeSpeedometer instance;
    return instance;
}

void CModeSpeedometer::Render2D(int scrWidth, int scrHeight)
{
    int stringWidth;
    const int speedometerMargin = 35;

    g_ScreenText.Clear();
    g_ScreenText.PushPrintf("%.2f", g_pPlayerMove->velocity.Length2D());
    stringWidth = GetStringWidth(g_ScreenText.StringAt(0));

    DrawStringStack(
        (scrWidth / 2) + stringWidth / 2, 
        (scrHeight / 2) + speedometerMargin, 
        g_ScreenText
    );
}
