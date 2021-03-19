#pragma once
#include "utils.h"
#include "hooks.h"
#include "cvars.h"
#include "app_version.h"
#include "exception.h"
#include "buildinfo.h"
#include "display_mode.h"
#include "string_stack.h"
#include <stdint.h>

class CApplication
{
public:
    static CApplication &GetInstance();
    void Run();
    void DisplayModeRender2D();
    void DisplayModeRender3D();
    bool KeyInput(int keyDown, int keyCode, const char *bindName);
    inline const SCREENINFO& GetScreenInfo() const { return m_ScreenInfo; };

private:
    CApplication() : m_StringStack(128) {};
    ~CApplication() {};

    void FindTimescaleConVar(const moduleinfo_t &engineLib);
    void PrintTitleText();
    void SetupConVars(moduleinfo_t &engineLib);
    void AssignDisplayMode();
    void UpdateScreenInfo();

    SCREENINFO m_ScreenInfo;
    CStringStack m_StringStack;
    IDisplayMode *m_pDisplayMode;
};
extern CApplication &g_Application;
