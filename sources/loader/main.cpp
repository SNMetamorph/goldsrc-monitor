#include "util.h"
#include "exception.h"
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
	if (processID > 0)
	{
		processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
		if (!processHandle)
			EXCEPT("unable to open game process");
	}
	else
		EXCEPT("unable to found game process, try to run game");
}

static void InjectLibrary(HANDLE process_handle)
{
	// getting full path to library
	wchar_t libPath[MAX_PATH];
	GetFullPathName(LIBRARY_NAME, MAX_PATH, libPath, NULL);
	size_t pathSize = (wcslen(libPath) + 1) * sizeof(libPath[0]);

	// check for desired library exists
	if (!PathFileExists(libPath))
		EXCEPT("library file not found");

	// getting address of LoadLibrary in game process
	size_t			funcOffset;
	HMODULE			k32LibHandle;
	moduleinfo_t	k32LibInfo;

	/*
		it's simple method to get address of function from remote process, 
		and will work in most cases, if kernel32.dll from game process 
		isn't differ with same library from loader
	*/
	funcOffset   = GetFunctionOffset(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	k32LibHandle = FindProcessModule(process_handle, L"kernel32.dll");
	if (!k32LibHandle)
		EXCEPT("module handle not found");

	if (!GetModuleInfo(process_handle, k32LibHandle, k32LibInfo))
		EXCEPT("module info getting failed");

	LPTHREAD_START_ROUTINE funcAddr = (LPTHREAD_START_ROUTINE)(
		(size_t)k32LibInfo.baseAddr + funcOffset
	);

	// allocating memory for library path in game process
	void *pathAddr = VirtualAllocEx(
		process_handle, 
		NULL, 
		pathSize,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	if (!pathAddr)
		EXCEPT("unable to allocate memory in game process");

	// writing string to game process
	size_t writtenBytes = 0;
	WriteProcessMemory(
		process_handle, pathAddr, 
		libPath, pathSize,
		(SIZE_T*)&writtenBytes
	);

	if (pathSize != writtenBytes)
		EXCEPT("writing to remote process failed");

	// creating thread in game process and invoking LoadLibrary function
	HANDLE threadID = CreateRemoteThread(process_handle, 
		0, 0, 
		funcAddr, pathAddr, 
		0, 0
	);

	if (!threadID)
		EXCEPT("unable to create remote thread");
}

static void PrintTitleText()
{
    system("cls");
    system("color 02");
    cout << 
    "\n"
    " GoldSrc Monitor | version 1.1\n"
    " ------------------------------\n"
    " WARNING: This stuff is untested on VAC-secured\n"
    " servers, therefore there is a risk to get VAC ban\n"
    " while using it on VAC-secured servers.\n"
    "\n"
    << endl;
}

int main(int argc, char *argv[])
{
	while (true)
	{
		HANDLE gameProc;
        PrintTitleText();

		try
		{
			gameProc = NULL;
			OpenGameProcess(gameProc);
			if (!FindProcessModule(gameProc, LIBRARY_NAME))
			{
				InjectLibrary(gameProc);
				Sleep(300);
				if (FindProcessModule(gameProc, LIBRARY_NAME))
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
		CloseHandle(gameProc);
	}

	cout << "Program will be closed 3 seconds later" << endl;
	Sleep(3000);
	return 0;
}