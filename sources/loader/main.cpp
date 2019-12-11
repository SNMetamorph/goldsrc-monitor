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
	cout << "Press any key to try again..." << endl;
	getchar();
}

static void OpenGameProcess(HANDLE &process_handle)
{
	int pid = FindProcessID(PROCESS_NAME);
	if (pid > 0)
	{
		process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if (!process_handle)
			EXCEPT("unable to open game process");
	}
	else
		EXCEPT("unable to found game process, try to run game");
}

static void InjectLibrary(HANDLE process_handle)
{
	// getting full path to library
	wchar_t lib_path[MAX_PATH];
	GetFullPathName(LIBRARY_NAME, MAX_PATH, lib_path, NULL);
	size_t path_size = (wcslen(lib_path) + 1) * sizeof(wchar_t);

	// check for desired library exists
	if (!PathFileExists(lib_path))
		EXCEPT("library file not found");

	// getting address of LoadLibrary in game process
	size_t			func_offset;
	HMODULE			k32lib_handle;
	module_info_t	k32lib_info;

	/*
		it's simple method to get address of function from remote process, 
		and will work in most cases, if kernel32.dll from game process 
		isn't differ with same library from loader
	*/
	func_offset	= GetFunctionOffset(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	k32lib_handle = FindProcessModule(process_handle, L"kernel32.dll");
	if (!k32lib_handle)
		EXCEPT("module handle not found");

	if (!GetModuleInfo(process_handle, k32lib_handle, k32lib_info))
		EXCEPT("module info getting failed");

	LPTHREAD_START_ROUTINE func_addr = (LPTHREAD_START_ROUTINE)(
		(size_t)k32lib_info.base_addr + func_offset
	);

	// allocating memory for library path in game process
	void *path_addr = VirtualAllocEx(
		process_handle, 
		NULL, 
		path_size,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	if (!path_addr)
		EXCEPT("unable to allocate memory in game process");

	// writing string to game process
	size_t written_bytes = 0;
	WriteProcessMemory(
		process_handle, path_addr, 
		lib_path, path_size,
		(SIZE_T*)&written_bytes
	);

	if (path_size != written_bytes)
		EXCEPT("writing to remote process failed");

	// creating thread in game process and invoking LoadLibrary function
	HANDLE thread_id = CreateRemoteThread(process_handle, 
		0, 0, 
		func_addr, path_addr, 
		0, 0
	);

	if (!thread_id)
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

	cout << "Program will be closed 3 seconds later..." << endl;
	Sleep(3000);
	return 0;
}