#pragma once
#include "app_info.h"
#include <stdint.h>
#include <string>
#include <Windows.h>

class CApplication
{
public:
    static CApplication &GetInstance();
    int Run(int argc, char *argv[]);

private:
    CApplication() {};
    ~CApplication() {};

    void ParseParameters(int argc, char *argv[]);
    void StartMainLoop();
    void ReportError(const std::string &msg);
    bool IsLibraryInjected(HANDLE procHandle);
    bool IsGameLoaded(HWND windowHandle, int timeout);
    HWND FindGameWindow(HANDLE procHandle);
    HANDLE OpenGameProcess();
    char *WritePathString(HANDLE procHandle, const std::string &libPath);
    void InjectLibrary(HANDLE procHandle);
    void PrintTitleText();

    size_t m_iInjectDelay = 3000;
    std::string m_szProcessName = DEFAULT_PROCESS_NAME;
    std::string m_szLibraryName = DEFAULT_LIBRARY_NAME;
};
