#include "application.h"
#include "utils.h"
#include "exception.h"
#include "app_info.h"
#include <iostream>

CApplication &CApplication::GetInstance()
{
    static CApplication instance;
    return instance;
}

int CApplication::Run(int argc, char *argv[])
{
    LPWSTR commandLine = GetCommandLineW();
    LPWSTR *argvArray = CommandLineToArgvW(commandLine, &argc);
    ParseParameters(argc, argvArray);
    StartMainLoop();

    std::cout << "Program will be closed 3 seconds later..." << std::endl;
    Sleep(3000);
    return 0;
}

void CApplication::ParseParameters(int argc, wchar_t *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::wstring parameter = argv[i];
        if (parameter.compare(L"+process_name") == 0)
        {
            std::wstring argument = argv[++i];
            m_szProcessName = argument;
            continue;
        }
        else if (parameter.compare(L"+library_name") == 0)
        {
            std::wstring argument = argv[++i];
            m_szLibraryName = argument;
            continue;
        }
        else if (parameter.compare(L"+inject_delay") == 0)
        {
            std::wstring argument = argv[++i];
            m_iInjectDelay = std::stoi(argument);
            continue;
        }
    }
}

void CApplication::StartMainLoop()
{
    HWND gameWindow;
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

            // try to find game window ten times
            for (int i = 0; i < 10; ++i)
            {
                gameWindow = FindGameWindow(processHandle);
                if (!gameWindow) {
                    Sleep(500);
                }
                else {
                    break;
                }
            }

            if (gameWindow)
            {
                // wait until game being loaded
                while (!IsGameLoaded(gameWindow, 500));
            }
            else 
            {
                std::cout << "Failed to find game window, waiting " << m_iInjectDelay << " ms" << std::endl;
                Sleep(m_iInjectDelay);
            }

            if (!IsLibraryInjected(processHandle))
            {
                InjectLibrary(processHandle);
                if (IsLibraryInjected(processHandle))
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
}

void CApplication::ReportError(const std::string &msg)
{
    std::cout << "ERROR: " << msg << std::endl;
    std::cout << "Press Enter to try again" << std::endl;
    std::cin.get();
}

bool CApplication::IsLibraryInjected(HANDLE procHandle)
{
    // TODO truncate library path and keep only library file name
    return Utils::FindProcessModule(procHandle, m_szLibraryName.c_str()) != NULL;
}

bool CApplication::IsGameLoaded(HWND windowHandle, int timeout)
{
    DWORD result;
    return SendMessageTimeout(windowHandle, WM_NULL, NULL, NULL, SMTO_BLOCK, timeout, &result) != NULL;
}

HWND CApplication::FindGameWindow(HANDLE procHandle)
{
    std::vector<HWND> windowList;
    int processID = GetProcessId(procHandle);
    if (processID)
    {
        Utils::GetProcessWindowList(processID, windowList);
        for (auto it = windowList.begin(); it != windowList.end();)
        {
            if (IsWindowEnabled(*it) && IsWindowVisible(*it)) {
                ++it;
            }
            else {
                it = windowList.erase(it); // remove invisible windows
            }
        }
        if (windowList.size() > 0) {
            return windowList[0]; // just pick first window handle
        }
    }
    return NULL;
}

HANDLE CApplication::OpenGameProcess()
{
    HANDLE processHandle;
    std::vector<int> processIds;
    const DWORD accessFlags = (
        PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_VM_OPERATION |
        PROCESS_CREATE_THREAD |
        PROCESS_QUERY_INFORMATION
    );

    while (true)
    {
        Utils::FindProcessByName(m_szProcessName.c_str(), processIds);
        if (processIds.size() > 0)
        {
            int processID = processIds[0];
            if (processIds.size() > 1)
            {
                size_t processNumber;
                std::wcout << L"There are several processes with same name." << std::endl;
                for (size_t i = 0; i < processIds.size(); ++i) {
                    std::wcout << L"(" << i + 1 << L") " << m_szProcessName << " (PID: " << processIds[i] << ")" << std::endl;
                }
                std::wcout << L"Choose the required process: " << std::endl;
                while (true) // repeat until valid value arrives
                {
                    std::cin >> processNumber;
                    if (processNumber > 0 && processNumber <= processIds.size())
                    {
                        processID = processIds[processNumber - 1];
                        break;
                    }
                }
            }

            processHandle = OpenProcess(accessFlags, false, processID);
            if (processHandle)
                return processHandle;
            else
                EXCEPT("unable to open game process");
        }
        Sleep(500);
    }
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


void CApplication::InjectLibrary(HANDLE procHandle)
{
    std::wstring    libraryPath;
    wchar_t         *pathStrRemote;
    HMODULE         k32LocalHandle;
    HMODULE			k32RemoteHandle;
    ModuleInfo	k32Info;
    HANDLE          threadHandle;
    size_t			funcOffset;
    uint8_t         *funcRemote;

    if (!Utils::FindLibraryPath(m_szLibraryName, libraryPath))
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
    k32RemoteHandle = Utils::FindProcessModule(procHandle, L"kernel32.dll");

    if (!k32RemoteHandle)
        EXCEPT("kernel32.dll remote handle not found");

    if (!Utils::GetModuleInfo(procHandle, k32RemoteHandle, k32Info))
        EXCEPT("GetModuleInfo() for remote kernel32.dll failed");

    // creating thread in game process and invoking LoadLibrary()
    std::cout << "Starting injection thread in remote process..." << std::endl;
    funcOffset = Utils::GetFunctionOffset(k32LocalHandle, "LoadLibraryW");
    funcRemote = k32Info.baseAddress + funcOffset;
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

    int exitCode = Utils::GetThreadExitCode(threadHandle);
    if (exitCode && exitCode != 0xC0000005)
        std::cout << "Injected library base address: 0x" << std::hex << exitCode << std::endl;
    else
        EXCEPT("remote thread failed to load library");
}

void CApplication::PrintTitleText()
{
    std::system("cls");
    std::system("color 02");
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
