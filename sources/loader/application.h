#pragma once
#include <string>
#include <Windows.h>
#include <shlwapi.h>

class CApplication
{
public:
    static CApplication &GetInstance();
    int Run(int argc, char *argv[]);

private:
    CApplication() {};
    ~CApplication() {};

    void ReportError(const char *msg);
    HANDLE OpenGameProcess();
    bool FindLibraryPath(std::wstring &libPath);
    wchar_t *WritePathString(HANDLE procHandle, const std::wstring &libPath);
    int GetFuncReturnCode(HANDLE threadHandle);
    void InjectLibrary(HANDLE procHandle);
    void PrintTitleText();

    std::wstring m_szProcessName = L"hl.exe";
    std::wstring m_szLibraryName = L"gsm-library.dll";
};
