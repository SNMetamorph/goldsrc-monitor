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

#include "stdafx.h"
#include "application.h"
#include "exception.h"
#include "hooks.h"
#include "sys_utils.h"

#include <Windows.h>
#include <stdint.h>

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        try 
        {
            SysUtils::InitCurrentLibraryHandle(hDllHandle);
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
                ex.GetFileName().c_str(),
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
