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
            m_szProcessName = argument;
            continue;
        }
        else if (parameter.compare("+library_name") == 0)
        {
            std::string argument = argv[++i];
            m_szLibraryName = argument;
            continue;
        }
        else if (parameter.compare("+inject_delay") == 0)
        {
            std::string argument = argv[++i];
            m_iInjectDelay = std::stoi(argument);
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
            InjectStatus status = m_pInjectStrategy->Start(m_iInjectDelay, m_szProcessName, m_szLibraryName);
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
    m_pInjectStrategy = std::make_unique<CWin32InjectStrategy>();
#endif
}
