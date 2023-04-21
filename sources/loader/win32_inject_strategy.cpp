#include "win32_inject_strategy.h"
#include "exception.h"
#include "sys_utils.h"
#include "utils.h"
#include <vector>
#include <iostream>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

class CWin32InjectStrategy::Impl
{
public:
    bool IsLibraryInjected(HANDLE procHandle, const std::string &libraryName);
    bool IsGameLoaded(HWND windowHandle, int timeout);
    HWND FindGameWindow(HANDLE procHandle);
    HANDLE OpenGameProcess(const std::string &processName);
    char *WritePathString(HANDLE procHandle, const std::string &libPath);
    void InjectLibrary(HANDLE procHandle, const std::string &libraryName);
    int GetThreadExitCode(HANDLE threadHandle);
    void GetProcessWindowList(DWORD processID, std::vector<HWND> &windowList);
};

CWin32InjectStrategy::CWin32InjectStrategy()
{
    m_pImpl = std::make_unique<CWin32InjectStrategy::Impl>();
}

CWin32InjectStrategy::~CWin32InjectStrategy()
{
}

InjectStatus CWin32InjectStrategy::Start(size_t injectDelayMsec, 
    const std::string &processName, const std::string &libraryName)
{
    HWND gameWindow;
    HANDLE processHandle;

	std::cout << "Target process name: " << processName.c_str() << std::endl;
    std::cout << "Waiting for starting game..." << std::endl;
    processHandle = m_pImpl->OpenGameProcess(processName);
    std::cout << "Game process found. Waiting for game loading..." << std::endl;

    // try to find game window ten times
    for (size_t i = 0; i < 10; ++i)
    {
        gameWindow = m_pImpl->FindGameWindow(processHandle);
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
        while (!m_pImpl->IsGameLoaded(gameWindow, 500));
    }
    else 
    {
        std::cout << "Failed to find game window, waiting " << injectDelayMsec << " ms" << std::endl;
        Sleep(injectDelayMsec);
    }

    if (!m_pImpl->IsLibraryInjected(processHandle, libraryName))
    {
        m_pImpl->InjectLibrary(processHandle, libraryName);
        if (m_pImpl->IsLibraryInjected(processHandle, libraryName))
        {
            std::cout << "Library successfully injected: check game console for more info" << std::endl;
            CloseHandle(processHandle);
            return InjectStatus::Success;
        }
        else 
        {
            CloseHandle(processHandle);
            EXCEPT("library injection performed, but module not found");
        }
    }
    else
    {
        CloseHandle(processHandle);
        std::cout << "Library already injected into game process, restart game and try again" << std::endl;
        return InjectStatus::AlreadyInjected;
    }
}

bool CWin32InjectStrategy::Impl::IsLibraryInjected(HANDLE procHandle, const std::string &libraryName)
{
    return SysUtils::FindModuleInProcess(procHandle, libraryName.c_str()) != NULL;
}

bool CWin32InjectStrategy::Impl::IsGameLoaded(HWND windowHandle, int timeout)
{
#ifdef APP_SUPPORT_64BIT // TODO instead check for WinSDK version
    DWORD_PTR result;
#else
    DWORD result;
#endif
    return SendMessageTimeout(windowHandle, WM_NULL, NULL, NULL, SMTO_BLOCK, timeout, &result) != NULL;
}

HWND CWin32InjectStrategy::Impl::FindGameWindow(HANDLE procHandle)
{
    std::vector<HWND> windowList;
    int processID = GetProcessId(procHandle);
    if (processID)
    {
        GetProcessWindowList(processID, windowList);
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

HANDLE CWin32InjectStrategy::Impl::OpenGameProcess(const std::string &processName)
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
        SysUtils::FindProcessIdByName(processName.c_str(), processIds);
        if (processIds.size() > 0)
        {
            int processID = processIds[0];
            if (processIds.size() > 1)
            {
                size_t processNumber;
                std::cout << "There are several processes with same name." << std::endl;
                for (size_t i = 0; i < processIds.size(); ++i) {
                    std::cout << "(" << i + 1 << L") " << processName << " (PID: " << processIds[i] << ")" << std::endl;
                }
                std::cout << "Choose the required process: " << std::endl;
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

char *CWin32InjectStrategy::Impl::WritePathString(HANDLE procHandle, const std::string &libPath)
{
    size_t writtenBytes = 0;
    char *pathRemoteAddr;

    // allocating memory in game process for library path string
    pathRemoteAddr = reinterpret_cast<char*>(VirtualAllocEx(
        procHandle,
        NULL,
        libPath.capacity(),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    ));

    if (!pathRemoteAddr) {
        return nullptr;
    }

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


void CWin32InjectStrategy::Impl::InjectLibrary(HANDLE procHandle, const std::string &libraryName)
{
    fs::path        libraryPath;
    char            *pathStrRemote;
    ModuleHandle    k32LocalHandle;
    ModuleHandle    k32RemoteHandle;
    HANDLE          threadHandle;
    size_t			funcOffset;
    uint8_t         *funcRemote;
    SysUtils::ModuleInfo k32Info;

    if (!Utils::FindLibraryAbsolutePath(libraryName, libraryPath))
        EXCEPT("library file not found, make sure that you unpacked program from archive");

    pathStrRemote = WritePathString(procHandle, libraryPath.string());
    if (!pathStrRemote)
        EXCEPT("unable to write library path string in game process memory");

    /*
    getting address of LoadLibrary() in game process
    it's simple method to get address of function in remote process
    and it will work in most cases, if kernel32.dll from game process
    isn't differ with same kernel32.dll from loader process
    */
    k32LocalHandle = GetModuleHandleA("kernel32.dll");
    k32RemoteHandle = SysUtils::FindModuleInProcess(procHandle, "kernel32.dll");

    if (!k32RemoteHandle) {
        EXCEPT("kernel32.dll remote handle not found");
    }

    if (!SysUtils::GetModuleInfo(procHandle, k32RemoteHandle, k32Info)) {
        EXCEPT("GetModuleInfo() for remote kernel32.dll failed");
    }

    // creating thread in game process and invoking LoadLibrary()
    std::cout << "Starting injection thread in remote process..." << std::endl;
    funcOffset = Utils::GetFunctionOffset(k32LocalHandle, "LoadLibraryA");
    funcRemote = k32Info.baseAddress + funcOffset;
    threadHandle = CreateRemoteThread(procHandle,
        0, 0,
        (LPTHREAD_START_ROUTINE)funcRemote, pathStrRemote,
        0, 0
    );

    if (!threadHandle) {
        EXCEPT("unable to create remote thread");
    }

    // wait for thread in game process exits
    if (WaitForSingleObject(threadHandle, 6000) == WAIT_TIMEOUT) {
        EXCEPT("library injection thread timed out");
    }

    int exitCode = GetThreadExitCode(threadHandle);
    if (exitCode && exitCode != 0xC0000005)
        std::cout << "Injected library base address: 0x" << std::hex << exitCode << std::endl;
    else
        EXCEPT("remote thread failed to load library");
}

int CWin32InjectStrategy::Impl::GetThreadExitCode(HANDLE threadHandle)
{
    DWORD exitCode;
    GetExitCodeThread(threadHandle, &exitCode);
    return exitCode;
}

void CWin32InjectStrategy::Impl::GetProcessWindowList(DWORD processID, std::vector<HWND> &windowList)
{
    HWND window = NULL;
    windowList.clear();
    do {
        DWORD checkProcessID;
        window = FindWindowEx(NULL, window, NULL, NULL);
        GetWindowThreadProcessId(window, &checkProcessID);
        if (processID == checkProcessID) {
            windowList.push_back(window);
        }
    } 
    while (window != NULL);
}
