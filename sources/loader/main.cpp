#include "util.h"
#include "exception.h"
#include <iostream>
#include <Windows.h>
#include <shlwapi.h>
using namespace std;

#define PROCESS_NAME L"hl.exe"
#define LIBRARY_NAME L"gsm-library.dll"

void report_error(const char *msg)
{
	cout << "ERROR: " << msg << endl;
	cout << "Press any key to try again..." << endl;
	getchar();
}

bool check_game_process(HANDLE &process_handle)
{
	// TODO: implement exceptions for suitable handling some situations 
	int pid = find_process(PROCESS_NAME);
	if (pid > 0)
	{
		process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if (process_handle)
			return true;
	}
	return false;
}

void inject_library(HANDLE process_handle)
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
	func_offset	= get_function_offset(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	k32lib_handle = find_process_module(process_handle, L"kernel32.dll");
	if (!k32lib_handle)
		EXCEPT("module handle not found");

	if (!get_module_info(process_handle, k32lib_handle, k32lib_info))
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

int main(int argc, char *argv[])
{
	while (true)
	{
		HANDLE game_proc;

		system("cls");
		system("color 02");
		cout << "####################################" << endl;
		cout << "#" << endl;
		cout << "#  GoldSrc Monitor | version 1.0" << endl;
		cout << "#  ---------------------------------" << endl;
		cout << "#  WARNING:  This stuff is untested" << endl;
		cout << "#  on VAC-secured servers, therefore" << endl;
		cout << "#  there is a risk to get VAC ban" << endl;
		cout << "#  while using it on VAC-secured" << endl;
		cout << "#  servers." << endl;
		cout << endl << endl;

		try
		{
			game_proc = NULL;
			if (check_game_process(game_proc))
			{
				if (!find_process_module(game_proc, LIBRARY_NAME))
				{
					inject_library(game_proc);
					Sleep(300);
					if (find_process_module(game_proc, LIBRARY_NAME))
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
			else
				report_error("hl.exe process not found, try to run the game");
		}
		catch (CException ex) {
			report_error(ex.getDescription());
		}
		CloseHandle(game_proc);
	}
	/*
	cout << "Press any key to exit or close window..." << endl;
	getchar();
	*/
	cout << "Program will be closed 3 seconds later..." << endl;
	Sleep(3000);
	return 0;
}