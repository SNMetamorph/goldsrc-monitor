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
#include <memory>
#include <stdint.h>

class CApplication
{
public:
    static CApplication &GetInstance();
    void Run();
    void DisplayModeRender2D();
    void DisplayModeRender3D();
    void HandleChangelevel();
    bool KeyInput(int keyDown, int keyCode, const char *bindName);
    inline const SCREENINFO& GetScreenInfo() const { return m_ScreenInfo; };

private:
    CApplication() {
        InitializeDisplayModes();
    };
    ~CApplication() {};

    void InitializeDisplayModes();
    void FindTimescaleConVar(const ModuleInfo &engineLib);
    void PrintTitleText();
    void InitializeConVars(ModuleInfo &engineLib);
    void SetCurrentDisplayMode();
    void UpdateScreenInfo();
    void UpdateSmoothFrametime();

    float m_flFrameTime;
    float m_flLastFrameTime;
    float m_flLastSysTime;
    CHooks m_Hooks;
    CBuildInfo m_BuildInfo;
    SCREENINFO m_ScreenInfo = { 0 };
    CStringStack m_StringStack = CStringStack(128);
    std::shared_ptr<IDisplayMode> m_pCurrentDisplayMode = nullptr;
    std::vector<std::shared_ptr<IDisplayMode>> m_pDisplayModes;
};
extern CApplication &g_Application;
