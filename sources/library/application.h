#pragma once
#include "utils.h"
#include "hooks.h"
#include "cvars.h"
#include "app_version.h"
#include "exception.h"
#include "build_info.h"
#include "display_mode.h"
#include "string_stack.h"
#include <stdint.h>

#include "displaymode_full.h"
#include "displaymode_measurement.h"
#include "displaymode_speedometer.h"
#include "displaymode_entityreport.h"
#include "displaymode_angletracking.h"

class CApplication
{
public:
    static CApplication &GetInstance();
    void Run();
    void DisplayModeRender2D();
    void DisplayModeRender3D();
    void CheckForChangelevel(float currTime);
    bool KeyInput(int keyDown, int keyCode, const char *bindName);
    inline const SCREENINFO& GetScreenInfo() const { return m_ScreenInfo; };

private:
    CApplication() : m_StringStack(128) {};
    ~CApplication() {};

    void HandleChangelevel();
    void FindTimescaleConVar(const moduleinfo_t &engineLib);
    void PrintTitleText();
    void SetupConVars(moduleinfo_t &engineLib);
    void AssignDisplayMode();
    void UpdateScreenInfo();

    CHooks m_Hooks;
    CBuildInfo m_BuildInfo;
    SCREENINFO m_ScreenInfo;
    CStringStack m_StringStack;
    IDisplayMode *m_pDisplayMode;
    CModeFull m_ModeFull;
    CModeSpeedometer m_ModeSpeedometer;
    CModeEntityReport m_ModeEntityReport;
    CModeMeasurement m_ModeMeasurement;
    CModeAngleTracking m_ModeAngleTracking;
};
extern CApplication &g_Application;
