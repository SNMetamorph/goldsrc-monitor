#include "displaymode_speedometer.h"
#include "client_module.h"
#include "utils.h"

void CModeSpeedometer::Render2D(int scrWidth, int scrHeight, CStringStack &screenText)
{
    int stringWidth;
    const int speedometerMargin = 35;

    screenText.Clear();
    screenText.PushPrintf("%.2f", g_pPlayerMove->velocity.Length2D());
    stringWidth = Utils::GetStringWidth(screenText.StringAt(0));

    Utils::DrawStringStack(
        (scrWidth / 2) + stringWidth / 2, 
        (scrHeight / 2) + speedometerMargin, 
        screenText
    );
}
