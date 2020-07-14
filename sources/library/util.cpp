#include "util.h"
#include "moduleinfo.h"
#include "globals.h" // temporary

#include <stdint.h>
#include <cstring>
#include <vector>
#include <Windows.h>
#include <Psapi.h>

void *FindPatternAddress(
    void *startAddr, void *endAddr, const char *pattern, const char *mask)
{
    bool isFailed;
    size_t maskLen;
    uint8_t *totalEndAddr;

    maskLen = strlen(mask);
    totalEndAddr = (uint8_t*)endAddr - maskLen;
    for (uint8_t *i = (uint8_t*)startAddr; i <= totalEndAddr; ++i)
    {
        isFailed = false;
        for (size_t j = 0; j < maskLen; ++j)
        {
            uint8_t maskByte = mask[j];
            uint8_t scanByte = *(i + j);
            uint8_t patternByte = pattern[j];

            if (maskByte != '?' && patternByte != scanByte)
            {
                isFailed = true;
                break;
            }
        }
        if (!isFailed)
            return i;
    }
    return nullptr;
}

HMODULE FindModuleByExport(HANDLE procHandle, const char *exportName)
{
    DWORD listSize;
    size_t modulesCount;
    std::vector<HMODULE> modulesList;

    // retrieve modules count
    listSize = 0;
    EnumProcessModules(procHandle, NULL, 0, &listSize);
    modulesCount = listSize / sizeof(HMODULE);

    if (modulesCount > 0)
        modulesList.resize(modulesCount);
    else
        return NULL;

    if (!EnumProcessModules(procHandle, modulesList.data(), listSize, &listSize))
        return NULL;

    for (size_t i = 0; i < modulesCount; ++i)
    {
        uint8_t *moduleAddr;
        uint32_t *nameOffsetList;
        PIMAGE_DOS_HEADER dosHeader;
        PIMAGE_NT_HEADERS peHeader;
        PIMAGE_EXPORT_DIRECTORY dllExports;

        moduleAddr  = (uint8_t*)modulesList[i];
        dosHeader   = (PIMAGE_DOS_HEADER)moduleAddr;
        peHeader    = (PIMAGE_NT_HEADERS)(moduleAddr + dosHeader->e_lfanew);
        dllExports  = (PIMAGE_EXPORT_DIRECTORY)(moduleAddr +
            peHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        if (!dllExports->AddressOfNames)
            continue;

        nameOffsetList = (uint32_t*)(moduleAddr + dllExports->AddressOfNames);
        for (size_t j = 0; j < dllExports->NumberOfNames; ++j)
        {
            const char *entryName = (const char *)(moduleAddr + nameOffsetList[j]);
            if (strcmp(entryName, exportName) == 0)
                return modulesList[i];
        }
    }
    return NULL;
}

bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo)
{
    MODULEINFO minfo;
    if (!GetModuleInformation(procHandle, moduleHandle, &minfo, sizeof(minfo)))
        return false;

    moduleInfo.baseAddr = (uint8_t*)minfo.lpBaseOfDll;
    moduleInfo.imageSize = minfo.SizeOfImage;
    moduleInfo.entryPointAddr = (uint8_t*)minfo.EntryPoint;
    return true;
}

void *FindMemoryInt32(void *startAddr, void *endAddr, uint32_t scanValue)
{
    void *valueAddr;
    HANDLE procHandle;
    uint32_t probeValue;
    uint32_t *totalEndAddr;

    valueAddr       = nullptr;
    procHandle      = GetCurrentProcess();
    totalEndAddr    = (uint32_t*)((size_t)endAddr - sizeof(scanValue));

    for (uint32_t *i = (uint32_t*)startAddr; i <= totalEndAddr; ++i)
    {
        if (!ReadProcessMemory(procHandle, i, &probeValue, sizeof(*i), NULL))
            continue;

        if (probeValue == scanValue)
        {
            valueAddr = i;
            break;
        }
    }
    return valueAddr;
}

void TraceLine(vec3_t &origin, vec3_t &dir, float lineLen, pmtrace_t *traceData)
{
    vec3_t lineStart;
    vec3_t lineEnd;
    cl_entity_t *localPlayer;

    lineStart   = origin;
    lineEnd     = lineStart + (dir * lineLen);
    localPlayer = g_pClientEngFuncs->GetLocalPlayer();

    g_pClientEngFuncs->pEventAPI->EV_SetUpPlayerPrediction(false, true);
    g_pClientEngFuncs->pEventAPI->EV_PushPMStates();
    g_pClientEngFuncs->pEventAPI->EV_SetSolidPlayers(localPlayer->index - 1);
    g_pClientEngFuncs->pEventAPI->EV_SetTraceHull(2);
    g_pClientEngFuncs->pEventAPI->EV_PlayerTrace(
        lineStart, lineEnd, PM_NORMAL,
        localPlayer->index, traceData
    );
    g_pClientEngFuncs->pEventAPI->EV_PopPMStates();
}

float TraceBBoxLine(
    const vec3_t &bboxMin, const vec3_t &bboxMax, 
    const vec3_t &lineStart, const vec3_t &lineEnd)
{
    vec3_t rayDirection;
    vec3_t invertedDir;
    vec3_t fractionMin;
    vec3_t fractionMax;
    vec3_t fractionNear;
    vec3_t fractionFar;
    float nearDotFract;
    float farDotFract;

    // ray equation
    // vector O + vector D * t
    // O - ray origin
    // D - ray direction
    // t - fraction
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

    const vec3_t &rayOrigin = lineStart;
    rayDirection = (lineEnd - lineStart);
    const float lineLength = rayDirection.Length();
    rayDirection = rayDirection.Normalize();

    invertedDir.x   = 1.f / rayDirection.x; 
    invertedDir.y   = 1.f / rayDirection.y; 
    invertedDir.z   = 1.f / rayDirection.z; 

    fractionMin.x   = (bboxMin.x - rayOrigin.x) * invertedDir.x;
    fractionMin.y   = (bboxMin.y - rayOrigin.y) * invertedDir.y;
    fractionMax.x   = (bboxMax.x - rayOrigin.x) * invertedDir.x;
    fractionMax.y   = (bboxMax.y - rayOrigin.y) * invertedDir.y;

    fractionNear.x  = min(fractionMin.x, fractionMax.x);
    fractionNear.y  = min(fractionMin.y, fractionMax.y);
    fractionFar.x   = max(fractionMin.x, fractionMax.x);
    fractionFar.y   = max(fractionMin.y, fractionMax.y);

    farDotFract     = fractionFar.x;
    nearDotFract    = fractionNear.x;

    // handle case when ray misses the box
    if (nearDotFract > fractionFar.y || fractionNear.y > farDotFract) 
        return 1.f;

    if (fractionNear.y > nearDotFract)
        nearDotFract = fractionNear.y;

    if (fractionFar.y < farDotFract)
        farDotFract = fractionFar.y;

    fractionMin.z   = (bboxMin.z - rayOrigin.z) * invertedDir.z;
    fractionMax.z   = (bboxMax.z - rayOrigin.z) * invertedDir.z;
    fractionFar.z   = max(fractionMin.z, fractionMax.z);
    fractionNear.z  = min(fractionMin.z, fractionMax.z);

    // another one
    if (nearDotFract > fractionFar.z || fractionNear.z > farDotFract)
        return 1.f;

    if (fractionNear.z > nearDotFract)
        nearDotFract = fractionNear.z;

    if (fractionFar.z < farDotFract)
        farDotFract = fractionFar.z;

    if (nearDotFract < 0.f)
        return 1.f;

    return nearDotFract / lineLength;
}

float GetCurrentSysTime()
{
    static LARGE_INTEGER	perfFreq;
    static LARGE_INTEGER	clockStart;
    LARGE_INTEGER		    currentTime;
    LONGLONG                timeDiff;

    if (!perfFreq.QuadPart)
    {
        QueryPerformanceFrequency(&perfFreq);
        QueryPerformanceCounter(&clockStart);
    }

    QueryPerformanceCounter(&currentTime);
    timeDiff = currentTime.QuadPart - clockStart.QuadPart;
    return (float)timeDiff / (float)perfFreq.QuadPart;
}
