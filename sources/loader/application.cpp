#include "application.h"
#include "util.h"
#include "exception.h"
#include "app_version.h"
#include <iostream>


CApplication &CApplication::GetInstance()
{
    static CApplication instance;
    return instance;
}

int CApplication::Run(int argc, char *argv[])
{
    HANDLE processHandle;
    while (true)
    {
        PrintTitleText();
        try
        {
            std::wcout << L"Target process name: " << m_szProcessName.c_str() << std::endl;
            std::cout << "Waiting for starting game..." << std::endl;
            processHandle = OpenGameProcess();
            std::cout << "Game process found. Waiting for game loading..." << std::endl;
            Sleep(3000);

            if (!FindProcessModule(processHandle, m_szLibraryName.c_str()))
            {
                InjectLibrary(processHandle);
                if (FindProcessModule(processHandle, m_szLibraryName.c_str()))
                {
                    std::cout << "Library successfully injected: check game console for more info" << std::endl;
                    break;
                }
                else
                    EXCEPT("library injection performed, but module not found");
            }
            else
            {
                std::cout << "Library already injected into game process, restart game and try again" << std::endl;
                break;
            }
        }
        catch (CException &ex)
        {
            ReportError(ex.GetDescription());
        }
        CloseHandle(processHandle);
    }

    std::cout << "Program will be closed 3 seconds later..." << std::endl;
    Sleep(3000);
    return 0;
}

void CApplication::ReportError(const char *msg)
{
    std::cout << "ERROR: " << msg << std::endl;
    std::cout << "Press Enter to try again" << std::endl;
    std::cin.get();
}

HANDLE CApplication::OpenGameProcess()
{
    HANDLE processHandle;
    const DWORD accessFlags = (
        PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_VM_OPERATION |
        PROCESS_CREATE_THREAD |
        PROCESS_QUERY_INFORMATION
    );

    while (true)
    {
        int processID = FindProcessID(m_szProcessName.c_str());
        if (processID > 0)
        {
            processHandle = OpenProcess(accessFlags, false, processID);
            if (processHandle)
                return processHandle;
            else
                EXCEPT("unable to open game process");
        }
        Sleep(500);
    }
}

bool CApplication::FindLibraryPath(std::wstring &libPath)
{
    libPath.reserve(MAX_PATH);
    GetFullPathNameW(
        m_szLibraryName.c_str(),
        libPath.capacity(),
        libPath.data(),
        NULL
    );

    if (PathFileExistsW(libPath.data()))
        return true;
    else
        return false;
}

wchar_t *CApplication::WritePathString(HANDLE procHandle, const std::wstring &libPath)
{
    size_t writtenBytes = 0;
    wchar_t *pathRemoteAddr;

    // allocating memory in game process for library path string
    pathRemoteAddr = (wchar_t *)VirtualAllocEx(
        procHandle,
        NULL,
        libPath.capacity(),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    if (!pathRemoteAddr)
        return nullptr;

    // writing string to game process
    WriteProcessMemory(
        procHandle, pathRemoteAddr,
        libPath.data(), libPath.capacity(),
        (SIZE_T *)&writtenBytes
    );

    if (libPath.capacity() != writtenBytes)
        return nullptr;

    return pathRemoteAddr;
}

int CApplication::GetFuncReturnCode(HANDLE threadHandle)
{
    DWORD exitCode;
    GetExitCodeThread(threadHandle, &exitCode);
    return exitCode;
}

void CApplication::InjectLibrary(HANDLE procHandle)
{
    std::wstring    libraryPath;
    wchar_t *pathStrRemote;
    HMODULE         k32LocalHandle;
    HMODULE			k32RemoteHandle;
    moduleinfo_t	k32Info;
    HANDLE          threadHandle;
    size_t			funcOffset;
    uint8_t *funcRemote;

    if (!FindLibraryPath(libraryPath))
        EXCEPT("library file not found, make sure that you unpacked program from archive");

    pathStrRemote = WritePathString(procHandle, libraryPath);
    if (!pathStrRemote)
        EXCEPT("unable to write library path string in game process memory");

    /*
    getting address of LoadLibrary() in game process
    it's simple method to get address of function in remote process
    and it will work in most cases, if kernel32.dll from game process
    isn't differ with same kernel32.dll from loader process
    */
    k32LocalHandle = GetModuleHandle(L"kernel32.dll");
    k32RemoteHandle = FindProcessModule(procHandle, L"kernel32.dll");

    if (!k32RemoteHandle)
        EXCEPT("kernel32.dll remote handle not found");

    if (!GetModuleInfo(procHandle, k32RemoteHandle, k32Info))
        EXCEPT("GetModuleInfo() for remote kernel32.dll failed");

    // creating thread in game process and invoking LoadLibrary()
    std::cout << "Starting injection thread in remote process..." << std::endl;
    funcOffset = GetFunctionOffset(k32LocalHandle, "LoadLibraryW");
    funcRemote = k32Info.baseAddr + funcOffset;
    threadHandle = CreateRemoteThread(procHandle,
        0, 0,
        (LPTHREAD_START_ROUTINE)funcRemote, pathStrRemote,
        0, 0
    );

    if (!threadHandle)
        EXCEPT("unable to create remote thread");

    // wait for thread in game process exits
    if (WaitForSingleObject(threadHandle, 6000) == WAIT_TIMEOUT)
        EXCEPT("library injection thread timed out");

    int exitCode = GetFuncReturnCode(threadHandle);
    if (exitCode && exitCode != 0xC0000005)
        std::cout << "Injected library base address: 0x" << std::hex << exitCode << std::endl;
    else
        EXCEPT("remote thread failed to load library");
}

void CApplication::PrintTitleText()
{
    system("cls");
    system("color 02");
    printf(
        "\n"
        " GoldSrc Monitor | version %d.%d\n"
        " ------------------------------\n"
        " WARNING: This stuff is untested on VAC-secured\n"
        " servers, therefore there is a risk to get VAC ban\n"
        " while using it on VAC-secured servers.\n"
        "\n", APP_VERSION_MAJOR, APP_VERSION_MINOR
    );
}