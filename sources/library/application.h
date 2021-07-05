#pragma once
#include "utils.h"
#include "hooks.h"
#include "cvars.h"
#include "app_info.h"
#include "exception.h"
#include "build_info.h"
#include "display_mode.h"
#include "string_stack.h"
#include <vector>
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
    CApplication() {
        InitializeDisplayModes();
    };
    ~CApplication() {};

    void InitializeDisplayModes();
    void HandleChangelevel();
    void FindTimescaleConVar(const ModuleInfo &engineLib);
    void PrintTitleText();
    void InitializeConVars(ModuleInfo &engineLib);
    void SetCurrentDisplayMode();
    void UpdateScreenInfo();

    CHooks m_Hooks;
    CBuildInfo m_BuildInfo;
    SCREENINFO m_ScreenInfo = { 0 };
    CStringStack m_StringStack = CStringStack(128);
    IDisplayMode *m_pCurrentDisplayMode = nullptr;
    std::vector<IDisplayMode *> m_pDisplayModes;
};
extern CApplication &g_Application;
