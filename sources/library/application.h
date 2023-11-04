/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#pragma once
#include "utils.h"
#include "hooks.h"
#include "cvars.h"
#include "sys_utils.h"
#include "app_info.h"
#include "exception.h"
#include "build_info.h"
#include "display_mode.h"
#include "primitives_renderer.h"
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
    const SCREENINFO& GetScreenInfo() const { return m_screenInfo; };
    auto GetRenderer() const { return m_primitivesRenderer; };

private:
    CApplication() {
        InitializeDisplayModes();
        InitializePrimitivesRenderer();
    };
    ~CApplication() {};

    void InitializeDisplayModes();
    void InitializePrimitivesRenderer();
    void FindTimescaleConVar(const SysUtils::ModuleInfo &engineLib);
    void PrintTitleText();
    void InitializeConVars(const SysUtils::ModuleInfo &engineLib);
    void SetCurrentDisplayMode();
    void UpdateScreenInfo();
    void UpdateSmoothFrametime();

    float m_frameTime;
    float m_lastFrameTime;
    float m_lastClientTime;
    CHooks m_hooks;
    CBuildInfo m_buildInfo;
    SCREENINFO m_screenInfo = { 0 };
    CStringStack m_stringStack = CStringStack(128);
    std::shared_ptr<IDisplayMode> m_currentDisplayMode = nullptr;
    std::shared_ptr<IPrimitivesRenderer> m_primitivesRenderer = nullptr;
    std::vector<std::shared_ptr<IDisplayMode>> m_displayModes;
};
extern CApplication &g_Application;
