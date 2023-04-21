#pragma once
#include "string_stack.h"
#include "memory_pattern.h"
#include "bounding_box.h"
#include "display_mode.h"
#include "color.h"
#include "stdafx.h"
#include <stdint.h>
#include <string>

namespace Utils
{
    inline bool IsSymbolAlpha(char symbol) { return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'); };
    inline bool IsSymbolDigit(char symbol) { return (symbol >= '0' && symbol <= '9'); };
    inline bool IsSymbolSpace(char symbol) { return (symbol == ' '); };

    void *FindMemoryPointer(void *startAddr, void *endAddr, void *scanValue);
    void *FindPatternAddress(void *startAddr, void *endAddr, const CMemoryPattern &pattern);
    void *FindJmpFromAddress(void *startAddr, void *endAddr, void *targetAddress);
    uint8_t *UnwrapJmp(uint8_t *opcodeAddr);

    float GetCurrentSysTime();
    DisplayModeIndex GetCurrentDisplayMode();

    cvar_t *RegisterConVar(const char *name, const char *value, int flags);
    int GetStringWidth(const char *str);
    bool WorldToScreen(int w, int h, int &x, int &y, const vec3_t &origin);
    void DrawStringStack(int marginRight, int marginUp, const CStringStack &stringStack);
    void DrawCuboid(const vec3_t &origin, const vec3_t &centerOffset, const vec3_t &angles, const vec3_t &size, Color color);
    int DrawString3D(const vec3_t &origin, const char *text, int r, int g, int b);
    void GetEntityModelName(int entityIndex, std::string &modelName);
    std::string GetCurrentMapName();
    const char *GetMovetypeName(int moveType);
    const char *GetRenderModeName(int renderMode);
    const char *GetRenderFxName(int renderFx);
    bool IsGameDirEquals(const char *gameDir);
    void Snprintf(std::string &result, const char *format, ...);

    vec3_t GetEntityVelocityApprox(int entityIndex, int approxStep = 22);
    void GetEntityBoundingBox(int entityIndex, CBoundingBox &bbox);
    void TraceLine(vec3_t &origin, vec3_t &dir, float lineLen, pmtrace_t *traceData, int ignoredEnt = -1);
    float TraceBBoxLine(const CBoundingBox &bbox, const vec3_t &lineStart, const vec3_t &lineEnd);
};
