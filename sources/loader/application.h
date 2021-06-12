#pragma once
#include "app_version.h"
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

    void ParseParameters(int argc, wchar_t *argv[]);
    void StartMainLoop();
    void ReportError(const char *msg);
    bool IsLibraryInjected(HANDLE procHandle);
    bool IsGameLoaded(HWND windowHandle, int timeout);
    HWND FindGameWindow(HANDLE procHandle);
    HANDLE OpenGameProcess();
    wchar_t *WritePathString(HANDLE procHandle, const std::wstring &libPath);
    void InjectLibrary(HANDLE procHandle);
    void PrintTitleText();

    size_t m_iInjectDelay = 3000;
    std::wstring m_szProcessName = DEFAULT_PROCESS_NAME;
    std::wstring m_szLibraryName = DEFAULT_LIBRARY_NAME;
};
