#include "stdafx.h"
#include "application.h"
#include "exception.h"
#include "hooks.h"

#include <Windows.h>
#include <stdint.h>
#include <cstdio>

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        try 
        {
            g_Application.Run();
        }
        catch (CException &ex)
        {
            snprintf(
                ex.m_szMessageBuffer,
                sizeof(ex.m_szMessageBuffer),
                "ERROR [%s:%d]: %s\nReport about error to the project page.\n"
                "Link: github.com/SNMetamorph/goldsrc-monitor/issues/1",
                ex.GetFileName(),
                ex.GetLineNumber(),
                ex.GetDescription()
            );
            MessageBox(NULL, ex.m_szMessageBuffer, "GoldSrc Monitor", MB_OK | MB_ICONWARNING);
            return FALSE;
        }
    }
    else if (nReason == DLL_PROCESS_DETACH)
    {
    }

    return TRUE;
}
