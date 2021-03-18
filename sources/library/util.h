#pragma once
#include "moduleinfo.h"
#include "stdafx.h"
#include <stdint.h>
#include <Windows.h>

HMODULE FindModuleByExport(HANDLE procHandle, const char *exportName);
bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo);
void *FindMemoryInt32(void *startAddr, void *endAddr, uint32_t value);
void *FindPatternAddress(void *startAddr, void *endAddr, const char *pattern, const char *mask);
void GetEntityBbox(int entityIndex, vec3_t &bboxMin, vec3_t &bboxMax);
void TraceLine(vec3_t &origin, vec3_t &dir, float lineLen, pmtrace_t *traceData);
float TraceBBoxLine(
    const vec3_t &bboxMin, const vec3_t &bboxMax, 
    const vec3_t &lineStart, const vec3_t &lineEnd
);
float GetCurrentSysTime();
inline bool IsSymbolAlpha(char symbol)      { return (symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'); };
inline bool IsSymbolDigit(char symbol)      { return (symbol >= '0' && symbol <= '9'); };
inline bool IsSymbolSpace(char symbol)      { return (symbol == ' '); };
