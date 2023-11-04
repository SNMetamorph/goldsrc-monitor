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

#include "application.h"
#include "exception.h"
#include "app_info.h"
#include "sys_utils.h"
#include "win32_inject_strategy.h"
#include <iostream>

CApplication &CApplication::GetInstance()
{
    static CApplication instance;
    return instance;
}

int CApplication::Run(int argc, char *argv[])
{
    ParseParameters(argc, argv);
    InitInjectStrategy();
    StartMainLoop();

    std::cout << "Program will be closed 3 seconds later..." << std::endl;
    SysUtils::Sleep(3000);
    return 0;
}

void CApplication::ParseParameters(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string parameter = argv[i];
        if (parameter.compare("+process_name") == 0)
        {
            std::string argument = argv[++i];
            m_processName = argument;
            continue;
        }
        else if (parameter.compare("+library_name") == 0)
        {
            std::string argument = argv[++i];
            m_libraryName = argument;
            continue;
        }
        else if (parameter.compare("+inject_delay") == 0)
        {
            std::string argument = argv[++i];
            m_injectDelay = std::stoi(argument);
            continue;
        }
    }
}

void CApplication::StartMainLoop()
{
    while (true)
    {
        PrintTitleText();
        try
        {
            InjectStatus status = m_injectStrategy->Start(m_injectDelay, m_processName, m_libraryName);
            if (status == InjectStatus::Success || status == InjectStatus::AlreadyInjected) {
                break;
            }
        }
        catch (CException &ex) {
            ReportError(ex.GetDescription());
        }
    }
}

void CApplication::ReportError(const std::string &msg)
{
    std::cout << "ERROR: " << msg << std::endl;
    std::cout << "Press Enter to try again" << std::endl;
    std::cin.get();
}

void CApplication::PrintTitleText()
{
#ifdef _WIN32
    std::system("cls");
    std::system("color 02");
#else
    // TODO clear console, set desired color
#endif
    std::printf(
        "\n"
        "  %s - utility for mapping/scripting/researching games on GoldSrc engine\n"
        "  Version  : %d.%d\n"
        "  Compiled : %s\n"
        "  Link     : %s\n"
        "\n"
        "  WARNING: This stuff is untested on VAC-secured\n"
        "  servers, therefore there is a risk to get VAC ban\n"
        "  while using it on VAC-secured servers.\n"
        "\n", APP_TITLE_STR, APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_BUILD_DATE, APP_GITHUB_LINK
    );
}

void CApplication::InitInjectStrategy()
{
#ifdef _WIN32
    m_injectStrategy = std::make_unique<CWin32InjectStrategy>();
#endif
}
