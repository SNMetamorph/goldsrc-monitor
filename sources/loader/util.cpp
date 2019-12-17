#include "util.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <stdint.h>


int FindProcessID(const wchar_t *processName)
{
	HANDLE			processSnap;
	PROCESSENTRY32	processEntry;

	processEntry.dwSize	= sizeof(processEntry);
	processSnap			= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (processSnap == INVALID_HANDLE_VALUE) 
		return NULL;
	
	if (!Process32First(processSnap, &processEntry)) {
		CloseHandle(processSnap);
		return NULL;
	}

	do {
		if (wcscmp(processName, processEntry.szExeFile) == 0) 
			return processEntry.th32ProcessID;
	} 
	while (Process32Next(processSnap, &processEntry));
	CloseHandle(processSnap);
	return NULL;
}

HMODULE FindProcessModule(HANDLE procHandle, const wchar_t *moduleName)
{
	DWORD	bufferSize;
	HMODULE moduleStub;
	HMODULE moduleResult = NULL;
	wchar_t	modulePath[MAX_PATH];

	if (EnumProcessModules(procHandle, &moduleStub, sizeof(moduleStub), &bufferSize))
	{
		size_t	modulesCount = bufferSize / sizeof(HMODULE);
		HMODULE	*modules     = new HMODULE[modulesCount];
		EnumProcessModules(procHandle, modules, bufferSize, &bufferSize);

		for (size_t i = 0; i < modulesCount; ++i)
		{
			size_t pathLength = sizeof(modulePath) / sizeof(modulePath[0]);
			GetModuleFileNameEx(procHandle, modules[i], modulePath, pathLength);
			wchar_t *fileName = wcsrchr(modulePath, '\\') + 1;
			if (wcscmp(fileName, moduleName) == 0)
			{
				moduleResult = modules[i];
				break;
			}
		}
		delete[] modules;
	}
	return moduleResult;
}

size_t GetFunctionOffset(HMODULE moduleHandle, const char *funcName)
{
	uint8_t *funcAddr = (uint8_t *)GetProcAddress(moduleHandle, funcName);
	return (size_t)(funcAddr - (uint8_t*)moduleHandle);
}

bool GetModuleInfo(HANDLE proc_handle, HMODULE module_handle, moduleinfo_t &module_info)
{
	MODULEINFO moduleInfo;
	if (!GetModuleInformation(proc_handle, module_handle, &moduleInfo, sizeof(moduleInfo)))
		return false;

	module_info.baseAddr			= (uint8_t*)moduleInfo.lpBaseOfDll;
	module_info.imageSize			= moduleInfo.SizeOfImage;
	module_info.entryPointAddr	= (uint8_t*)moduleInfo.EntryPoint;
	return true;
}
