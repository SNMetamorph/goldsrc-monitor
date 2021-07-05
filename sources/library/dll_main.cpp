#include "stdafx.h"
#include "application.h"
#include "exception.h"
#include "hooks.h"

#include <Windows.h>
#include <stdint.h>

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        try 
        {
            g_Application.Run();
        }
        catch (const CException &ex)
        {
            std::string errorMsg(256, '\0');
            std::snprintf(
                errorMsg.data(),
                errorMsg.capacity(),
                "ERROR [%s:%d]: %s\nReport about error to the project page.\n"
                "Link: github.com/SNMetamorph/goldsrc-monitor/issues/1",
                ex.GetFileName(),
                ex.GetLineNumber(),
                ex.GetDescription().c_str()
            );
            MessageBox(NULL, errorMsg.c_str(), APP_TITLE_STR, MB_OK | MB_ICONWARNING);
            return FALSE;
        }
    }
    else if (nReason == DLL_PROCESS_DETACH)
    {
    }

    return TRUE;
}
