#pragma once
#include "function_hook.h"

class CHooks
{
public:
    typedef int(__cdecl *pfnRedraw_t)(float, int);
    typedef void(__cdecl *pfnPlayerMove_t)(playermove_t *, int);
    typedef int(__cdecl *pfnKeyEvent_t)(int, int, const char *);
    typedef void(__cdecl *pfnDrawTriangles_t)();
    typedef int(__cdecl *pfnIsThirdPerson_t)();
    typedef void(__cdecl *pfnCameraOffset_t)(float *);

    void Apply();
    void Remove();

private:
    void RevertHooks();
};
