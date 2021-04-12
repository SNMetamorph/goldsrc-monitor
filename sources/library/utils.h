#pragma once
#include "moduleinfo.h"
#include "string_stack.h"
#include "stdafx.h"
#include <stdint.h>
#include <string>
#include <Windows.h>

namespace Utils
{
    inline bool IsSymbolAlpha(char symbol) { return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'); };
    inline bool IsSymbolDigit(char symbol) { return (symbol >= '0' && symbol <= '9'); };
    inline bool IsSymbolSpace(char symbol) { return (symbol == ' '); };

    HMODULE FindModuleByExport(HANDLE procHandle, const char *exportName);
    bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo);
    void *FindMemoryInt32(void *startAddr, void *endAddr, uint32_t value);
    void *FindPatternAddress(void *startAddr, void *endAddr, const char *pattern, const char *mask);
    float GetCurrentSysTime();

    cvar_t *RegisterConVar(const char *name, const char *value, int flags);
    int GetStringWidth(const char *str);
    bool WorldToScreen(int w, int h, int &x, int &y, const vec3_t &origin);
    void DrawStringStack(int marginRight, int marginUp, const CStringStack &stringStack);
    void DrawEntityHull(const vec3_t &origin, const vec3_t &centerOffset, const vec3_t &angles, const vec3_t &size);
    void DrawString3D(const vec3_t &origin, const char *text, int r, int g, int b);
    void GetEntityModelName(int entityIndex, std::string &modelName);
    bool IsGameDirEquals(const char *gameDir);

    void GetEntityBbox(int entityIndex, vec3_t &bboxMin, vec3_t &bboxMax);
    void TraceLine(vec3_t &origin, vec3_t &dir, float lineLen, pmtrace_t *traceData, int ignoredEnt = -1);
    float TraceBBoxLine(
        const vec3_t &bboxMin, const vec3_t &bboxMax,
        const vec3_t &lineStart, const vec3_t &lineEnd
    );
};
