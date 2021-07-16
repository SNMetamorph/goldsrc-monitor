#include "utils.h"
#include "module_info.h"
#include "client_module.h"
#include "studio.h"
#include "application.h"
#include "matrix.h"

#include <stdint.h>
#include <cstring>
#include <vector>
#include <Windows.h>
#include <Psapi.h>
#include <gl/GL.h>

void *Utils::FindPatternAddress(void *startAddr, void *endAddr, const CMemoryPattern &pattern)
{
    size_t patternLen = pattern.GetLength();
    uint8_t *totalEndAddr = (uint8_t*)endAddr - patternLen;
    const uint8_t *patternStart = pattern.GetSignatureAddress();
    const int *maskStart = pattern.GetMaskAddress();
    for (uint8_t *i = (uint8_t*)startAddr; i <= totalEndAddr; ++i)
    {
        bool isFailed = false;
        for (size_t j = 0; j < patternLen; ++j)
        {
            const uint8_t scanByte = *(i + j);
            const uint8_t patternByte = *(patternStart + j);
            const bool shouldCheckByte = *(maskStart + j) != 0;
            if (shouldCheckByte && patternByte != scanByte)
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

bool Utils::GetLibraryDirectory(std::wstring &workingDir)
{
    workingDir.resize(MAX_PATH);
    GetModuleFileNameW(
        GetModuleHandleW(DEFAULT_LIBRARY_NAME), // TODO use module handle from DllMain here
        workingDir.data(),
        workingDir.capacity()
    );

    if (std::wcslen(workingDir.c_str()) > 1)
    {
        // remove file name
        workingDir.assign(workingDir.c_str());
        workingDir.erase(workingDir.find_last_of(L"/\\") + 1);
        workingDir.shrink_to_fit();
        return true;
    }
    else
        return false;
}

void Utils::GetGameProcessName(std::string &processName)
{
    processName.resize(MAX_PATH);
    GetModuleFileName(NULL, processName.data(), processName.capacity());
    processName.assign(processName.c_str());
    processName.erase(0, processName.find_last_of("/\\") + 1);
    processName.shrink_to_fit();
}

HMODULE Utils::FindModuleByExport(HANDLE procHandle, const char *exportName)
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

int Utils::GetStringWidth(const char *str)
{
    const SCREENINFO &screenInfo = g_Application.GetScreenInfo();
    int totalWidth = 0;
    for (char *i = (char *)str; *i; ++i)
        totalWidth += screenInfo.charWidths[*i];
    return totalWidth;
}

bool Utils::WorldToScreen(int w, int h, int &x, int &y, const vec3_t &origin)
{
    vec3_t screenCoords;
    if (!g_pClientEngfuncs->pTriAPI->WorldToScreen((float*)&origin.x, &screenCoords.x))
    {
        x = static_cast<int>((1.0f + screenCoords.x) * w * 0.5f);
        y = static_cast<int>((1.0f - screenCoords.y) * h * 0.5f);
        return true;
    }
    return false;
}

void *Utils::FindMemoryInt32(void *startAddr, void *endAddr, uint32_t scanValue)
{
    void *valueAddr = nullptr;
    uint32_t *totalEndAddr = (uint32_t *)endAddr - sizeof(scanValue);
    for (uint32_t *i = (uint32_t*)startAddr; i <= totalEndAddr; ++i)
    {
        if (i[0] == scanValue)
        {
            valueAddr = i;
            break;
        }
    }
    return valueAddr;
}

cvar_t *Utils::RegisterConVar(const char *name, const char *value, int flags)
{
    cvar_t *probe = g_pClientEngfuncs->pfnGetCvarPointer(name);
    if (probe)
        return probe;
    return g_pClientEngfuncs->pfnRegisterVariable(name, value, flags);
}

void Utils::DrawStringStack(int marginRight, int marginUp, const CStringStack &stringStack)
{
    int linesSkipped = 0;
    int maxStringWidth = 0;
    int stringCount = stringStack.GetStringCount();
    const int stringHeight = 15;
    const SCREENINFO &screenInfo = g_Application.GetScreenInfo();

    for (int i = 0; i < stringCount; ++i)
    {
        const char *textString = stringStack.StringAt(i);
        int stringWidth = GetStringWidth(textString);
        if (stringWidth > maxStringWidth)
            maxStringWidth = stringWidth;
    }

    for (int i = 0; i < stringCount; ++i)
    {
        const char *textString = stringStack.StringAt(i);
        g_pClientEngfuncs->pfnDrawString(
            screenInfo.iWidth - max(marginRight, maxStringWidth + 5),
            marginUp + (stringHeight * (i + linesSkipped)),
            textString,
            (int)ConVars::gsm_color_r->value,
            (int)ConVars::gsm_color_g->value,
            (int)ConVars::gsm_color_b->value
        );

        int lastCharIndex = strlen(textString) - 1;
        if (textString[lastCharIndex] == '\n')
            ++linesSkipped;
    }
}

void Utils::DrawEntityHull(const vec3_t &origin, const vec3_t &centerOffset, const vec3_t &angles, const vec3_t &size)
{
    const float colorR = 0.0f;
    const float colorG = 1.0f;
    const float colorB = 0.0f;
    const bool drawFaces = false;
    const bool drawEdges = true;
    const vec3_t bboxMin = vec3_t(0, 0, 0) - size / 2;
    Matrix4x4<vec_t> transformMat;
    
    // assumed that point (0, 0, 0) located in center of bbox
    vec3_t boxVertices[8] = {
        {bboxMin.x, bboxMin.y, bboxMin.z},
        {bboxMin.x + size.x, bboxMin.y, bboxMin.z},
        {bboxMin.x + size.x, bboxMin.y, bboxMin.z + size.z},
        {bboxMin.x + size.x, bboxMin.y + size.y, bboxMin.z + size.z},
        {bboxMin.x + size.x, bboxMin.y + size.y, bboxMin.z},
        {bboxMin.x, bboxMin.y + size.y, bboxMin.z},
        {bboxMin.x, bboxMin.y + size.y, bboxMin.z + size.z},
        {bboxMin.x, bboxMin.y, bboxMin.z + size.z}
    };

    // transform all vertices
    transformMat = Matrix4x4<vec_t>::CreateTranslate(centerOffset.x, centerOffset.y, centerOffset.z);
    if (angles.Length() >= 0.001f) // just little optimization
    {
        transformMat = Matrix4x4<vec_t>::CreateRotateX(angles[2]) * transformMat; // roll
        transformMat = Matrix4x4<vec_t>::CreateRotateY(-angles[0]) * transformMat; // pitch (inverted because of stupid quake bug)
        transformMat = Matrix4x4<vec_t>::CreateRotateZ(angles[1]) * transformMat; // yaw
    }
    transformMat = Matrix4x4<vec_t>::CreateTranslate(origin.x, origin.y, origin.z) * transformMat;

    for (int i = 0; i < 8; ++i) {
        boxVertices[i] = transformMat.MultiplyVector(boxVertices[i]);
    }

    g_pClientEngfuncs->pTriAPI->RenderMode(kRenderTransColor);
    g_pClientEngfuncs->pTriAPI->CullFace(TRI_NONE);
    glDisable(GL_TEXTURE_2D);

    if (drawFaces)
    {
        const int faceIndices[] = {
            7, 2, 1, 0, // 1 face
            0, 5, 6, 7, // 2 face
            5, 4, 3, 6, // 3 face
            2, 3, 4, 1, // 4 face
            7, 6, 3, 2, // 5 face
            1, 4, 5, 0  // 6 face
        };
        glBegin(GL_QUADS);
        glColor4f(colorR, colorG, colorB, 0.3f);
        for (int i = 0; i < sizeof(faceIndices) / sizeof(faceIndices[0]); ++i) {
            glVertex3fv(boxVertices[faceIndices[i]]);
        }
        glEnd();
    }
    
    if (drawEdges)
    {
        const int edgeIndices[] = {
            0, 1, 2, 3, 4, 5, 6, 7,
            0, 7, 2, 1, 4, 3, 6, 5
        };
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_LINE_LOOP);
        glColor4f(colorR, colorG, colorB, 1.0f);
        for (int i = 0; i < sizeof(edgeIndices) / sizeof(edgeIndices[0]); ++i) {
            glVertex3fv(boxVertices[edgeIndices[i]]);
        }
        glEnd();
        glEnable(GL_DEPTH_TEST);
    }

    g_pClientEngfuncs->pTriAPI->RenderMode(kRenderNormal);
    g_pClientEngfuncs->pTriAPI->CullFace(TRI_FRONT);
    glEnable(GL_TEXTURE_2D);
}

void Utils::DrawString3D(const vec3_t &origin, const char *text, int r, int g, int b)
{
    int screenX, screenY;
    const SCREENINFO &screenInfo = g_Application.GetScreenInfo();
    if (Utils::WorldToScreen(screenInfo.iWidth, screenInfo.iHeight, screenX, screenY, origin))
    {
        g_pClientEngfuncs->pfnDrawString(screenX, screenY, text, r, g, b);
    }
}

void Utils::GetEntityModelName(int entityIndex, std::string &modelName)
{
    const int maxClients = g_pClientEngfuncs->GetMaxClients();
    if (entityIndex > 0 && entityIndex <= maxClients) // is entity player
    {
        hud_player_info_t playerInfo;
        g_pClientEngfuncs->pfnGetPlayerInfo(entityIndex, &playerInfo);
        modelName = "models/player/" + std::string(playerInfo.model) + ".mdl";
    }
    else
    {
        cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
        modelName = entity->model->name;
    }
}

std::string Utils::GetCurrentMapName()
{
    std::string mapName = g_pClientEngfuncs->pfnGetLevelName();
    mapName.erase(0, mapName.find_last_of("/\\") + 1);
    mapName.erase(mapName.find_last_of("."));
    return mapName;
}

const char *Utils::GetMovetypeName(int moveType)
{
    switch (moveType)
    {
        case MOVETYPE_BOUNCE:           return "Bounce";
        case MOVETYPE_BOUNCEMISSILE:    return "Bounce-missle";
        case MOVETYPE_FLY:              return "Fly";
        case MOVETYPE_FLYMISSILE:       return "Fly-missle";
        case MOVETYPE_FOLLOW:           return "Follow";
        case MOVETYPE_NOCLIP:           return "Noclip";
        case MOVETYPE_NONE:             return "None";
        case MOVETYPE_PUSH:             return "Push";
        case MOVETYPE_PUSHSTEP:         return "Push-step";
        case MOVETYPE_STEP:             return "Step";
        case MOVETYPE_TOSS:             return "Toss";
        case MOVETYPE_WALK:             return "Walk";
        default:                        return "Unknown";
    }
}

const char *Utils::GetRenderModeName(int renderMode)
{
    switch (renderMode)
    {
        case kRenderNormal:         return "Normal";
        case kRenderTransColor:     return "Trans. color";
        case kRenderTransTexture:   return "Trans. texture";
        case kRenderGlow:           return "Glow";
        case kRenderTransAlpha:     return "Trans. alpha";
        case kRenderTransAdd:       return "Trans. additive";
        default:                    return "Unknown";
    }
}

const char *Utils::GetRenderFxName(int renderFx)
{
    switch (renderFx)
    {
        case kRenderFxNone:             return "None";
        case kRenderFxPulseSlow:        return "Pulse (slow)";
        case kRenderFxPulseFast:        return "Pulse (fast)";
        case kRenderFxPulseSlowWide:    return "Pulse (slow wide)";
        case kRenderFxPulseFastWide:    return "Pulse (fast wide)";
        case kRenderFxFadeSlow:         return "Fade (slow)";
        case kRenderFxFadeFast:         return "Fade (fast)";
        case kRenderFxSolidSlow:        return "Solid (slow)";
        case kRenderFxSolidFast:        return "Solid (fast)";
        case kRenderFxStrobeSlow:       return "Strobe (slow)";
        case kRenderFxStrobeFast:       return "Strobe (fast)";
        case kRenderFxStrobeFaster:     return "Strobe (faster)";
        case kRenderFxFlickerSlow:      return "Flicker (slow)";
        case kRenderFxFlickerFast:      return "Flicker (fast)";
        case kRenderFxNoDissipation:    return "No dissipation";
        case kRenderFxDistort:          return "Distort";
        case kRenderFxHologram:         return "Hologram";
        case kRenderFxDeadPlayer:       return "Dead player";
        case kRenderFxExplode:          return "Explode";
        case kRenderFxGlowShell:        return "Glow shell";
        case kRenderFxClampMinScale:    return "Clamp min. scale";
        case kRenderFxLightMultiplier:  return "Light multiplier";
        default:                        return "Unknown";
    }
}

bool Utils::IsGameDirEquals(const char *gameDir)
{
    const char *gameDirReal = g_pClientEngfuncs->pfnGetGameDirectory();
    return strcmp(gameDirReal, gameDir) == 0;
}

void Utils::TraceLine(vec3_t &origin, vec3_t &dir, float lineLen, pmtrace_t *traceData, int ignoredEnt)
{
    vec3_t lineStart;
    vec3_t lineEnd;
    cl_entity_t *localPlayer;

    lineStart   = origin;
    lineEnd     = lineStart + (dir * lineLen);
    localPlayer = g_pClientEngfuncs->GetLocalPlayer();

    if (ignoredEnt < 0)
        ignoredEnt = localPlayer->index;

    g_pClientEngfuncs->pEventAPI->EV_SetUpPlayerPrediction(false, true);
    g_pClientEngfuncs->pEventAPI->EV_PushPMStates();
    g_pClientEngfuncs->pEventAPI->EV_SetSolidPlayers(localPlayer->index - 1);
    g_pClientEngfuncs->pEventAPI->EV_SetTraceHull(2);
    g_pClientEngfuncs->pEventAPI->EV_PlayerTrace(
        lineStart, lineEnd, PM_NORMAL,
        ignoredEnt, traceData
    );
    g_pClientEngfuncs->pEventAPI->EV_PopPMStates();
}

float Utils::TraceBBoxLine(const CBoundingBox &bbox, const vec3_t &lineStart, const vec3_t &lineEnd)
{
    vec3_t invertedDir;
    vec3_t rayDirection;
    vec3_t fractionMin;
    vec3_t fractionMax;
    vec3_t fractionNear;
    vec3_t fractionFar;
    float nearDotFract;
    float farDotFract;
    const vec3_t &bboxMaxs = bbox.GetMaxs();
    const vec3_t &bboxMins = bbox.GetMins();

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

    fractionMin.x   = (bboxMins.x - rayOrigin.x) * invertedDir.x;
    fractionMin.y   = (bboxMins.y - rayOrigin.y) * invertedDir.y;
    fractionMax.x   = (bboxMaxs.x - rayOrigin.x) * invertedDir.x;
    fractionMax.y   = (bboxMaxs.y - rayOrigin.y) * invertedDir.y;

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

    fractionMin.z   = (bboxMins.z - rayOrigin.z) * invertedDir.z;
    fractionMax.z   = (bboxMaxs.z - rayOrigin.z) * invertedDir.z;
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

vec3_t Utils::GetEntityVelocityApprox(int entityIndex, int approxStep)
{
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);
    if (entity)
    {
        const int currIndex = entity->current_position;
        position_history_t &currState = entity->ph[currIndex & HISTORY_MASK];
        position_history_t &prevState = entity->ph[(currIndex - approxStep) & HISTORY_MASK];
        float timeDelta = currState.animtime - prevState.animtime;
        if (fabs(timeDelta) > 0.0f)
        {
            vec3_t originDelta = currState.origin - prevState.origin;
            return originDelta / timeDelta; 
        }
    }
    return vec3_t(0, 0, 0);
}

void Utils::GetEntityBoundingBox(int entityIndex, CBoundingBox &bbox)
{
    int seqIndex;
    vec3_t hullSize;
    studiohdr_t *mdlHeader;
    mstudioseqdesc_t *seqDesc;
    cl_entity_t *entity = g_pClientEngfuncs->GetEntityByIndex(entityIndex);

    if (!entity)
    {
        bbox = CBoundingBox(vec3_t(0, 0, 0));
        return;
    }
    else
    {
        const vec3_t centerOffset = (entity->curstate.mins + entity->curstate.maxs) / 2.f;
        const vec3_t entityOrigin = entity->origin + centerOffset;
        if (entity->model && entity->model->type == mod_studio)
        {
            mdlHeader = (studiohdr_t *)entity->model->cache.data;
            seqDesc = (mstudioseqdesc_t *)((char *)mdlHeader + mdlHeader->seqindex);
            seqIndex = entity->curstate.sequence;
            hullSize = seqDesc[seqIndex].bbmax - seqDesc[seqIndex].bbmin;
        }
        else {
            hullSize = entity->curstate.maxs - entity->curstate.mins;
        }
        bbox = CBoundingBox(hullSize);
        bbox.SetCenterToPoint(entityOrigin);
    }
}

float Utils::GetCurrentSysTime()
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
