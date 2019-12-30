#include "util.h"
#include "exception.h"
#include "app_version.h"
#include <iostream>
#include <Windows.h>
#include <shlwapi.h>
using namespace std;

#define PROCESS_NAME L"hl.exe"
#define LIBRARY_NAME L"gsm-library.dll"

static void ReportError(const char *msg)
{
	cout << "ERROR: " << msg << endl;
	cout << "Press Enter to try again" << endl;
	cin.get();
}

static void OpenGameProcess(HANDLE &processHandle)
{
	int processID = FindProcessID(PROCESS_NAME);
    const DWORD accessFlags = (
        PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_VM_OPERATION |
        PROCESS_CREATE_THREAD |
        PROCESS_QUERY_INFORMATION
    );

	if (processID > 0)
	{
		processHandle = OpenProcess(accessFlags, false, processID);
		if (!processHandle)
			EXCEPT("unable to open game process");
	}
	else
		EXCEPT("unable to found game process, try to run game");
}

static void InjectLibrary(HANDLE procHandle)
{
    HANDLE          threadID;
    size_t			funcOffset;
    size_t          writtenBytes;
    void            *pathRemoteAddr;
    HMODULE         k32LocalHandle;
    HMODULE			k32RemoteHandle;
    moduleinfo_t	k32Info;
	static wchar_t  libPath[MAX_PATH];
    const size_t    pathSize = sizeof(libPath);
    LPTHREAD_START_ROUTINE funcRemoteAddr;

	GetFullPathName(LIBRARY_NAME, MAX_PATH, libPath, NULL);
	if (!PathFileExists(libPath))
		EXCEPT("library file not found");

	/*
        getting address of LoadLibrary() in game process
        ----------------------------------------------
		it's simple method to get address of function from remote process 
		and will work in most cases, if kernel32.dll from game process 
		isn't differ with same library from loader
	*/
    k32LocalHandle  = GetModuleHandle(L"kernel32.dll");
	k32RemoteHandle = FindProcessModule(procHandle, L"kernel32.dll");

	if (!k32RemoteHandle)
		EXCEPT("kernel32.dll remote handle not found");

	if (!GetModuleInfo(procHandle, k32RemoteHandle, k32Info))
		EXCEPT("GetModuleInfo() for remote kernel32.dll failed");

	// allocating memory in game process for library path
	pathRemoteAddr = VirtualAllocEx(
		procHandle, 
		NULL, 
		pathSize,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	if (!pathRemoteAddr)
		EXCEPT("unable to allocate memory in game process");

	// writing string to game process
	writtenBytes = 0;
	WriteProcessMemory(
		procHandle, pathRemoteAddr, 
		libPath, pathSize,
		(SIZE_T*)&writtenBytes
	);

	if (pathSize != writtenBytes)
		EXCEPT("unable to write library path");

	// creating thread in game process and invoking LoadLibrary()
    funcOffset      = GetFunctionOffset(k32LocalHandle, "LoadLibraryW");
    funcRemoteAddr  = (LPTHREAD_START_ROUTINE)(k32Info.baseAddr + funcOffset);
	threadID        = CreateRemoteThread(procHandle, 
		0, 0, 
		funcRemoteAddr, pathRemoteAddr, 
		0, 0
	);

	if (!threadID)
		EXCEPT("unable to create remote thread");
}

static void PrintTitleText()
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

int main(int argc, char *argv[])
{
	while (true)
	{
		HANDLE gameProcess;
        PrintTitleText();

		try
		{
			gameProcess = NULL;
			OpenGameProcess(gameProcess);
			if (!FindProcessModule(gameProcess, LIBRARY_NAME))
			{
				InjectLibrary(gameProcess);
				Sleep(300);
				if (FindProcessModule(gameProcess, LIBRARY_NAME))
				{
					cout << "Library successfully injected: check game console for more info" << endl;
					break;
				}
				else
					EXCEPT("library injection performed, but nothing changed");
			}
			else
			{
				cout << "Library already injected into game process, restart game and try again" << endl;
				break;
			}
		}
		catch (CException &ex) 
        {
			ReportError(ex.GetDescription());
		}
		CloseHandle(gameProcess);
	}

	cout << "Program will be closed 3 seconds later" << endl;
	Sleep(3000);
	return 0;
}