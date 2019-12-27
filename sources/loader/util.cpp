#include "util.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <stdint.h>
#include <vector>

int FindProcessID(const wchar_t *processName)
{
    int             processID;
	HANDLE			processSnap;
	PROCESSENTRY32	processEntry;

    processID = NULL;
	processEntry.dwSize	= sizeof(processEntry);
	processSnap	= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (processSnap != INVALID_HANDLE_VALUE)
    {
        if (Process32First(processSnap, &processEntry))
        {
            do {
                if (wcscmp(processName, processEntry.szExeFile) == 0)
                {
                    processID = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(processSnap, &processEntry));
        }
        CloseHandle(processSnap);
    }
	return processID;
}

HMODULE FindProcessModule(HANDLE procHandle, const wchar_t *moduleName)
{
	DWORD	listSize;
    size_t  modulesCount;
	HMODULE moduleHandle;
    std::vector<HMODULE> modulesList;
	static wchar_t modulePath[MAX_PATH];
    const size_t pathLength = 
        sizeof(modulePath) / sizeof(modulePath[0]);

    EnumProcessModules(procHandle, NULL, 0, &listSize);
    moduleHandle = NULL;
	modulesCount = listSize / sizeof(HMODULE);

    if (modulesCount > 0)
        modulesList.resize(modulesCount);
    else
        return NULL;

	if (!EnumProcessModules(procHandle, modulesList.data(), listSize, &listSize))
        return NULL;

	for (size_t i = 0; i < modulesCount; ++i)
    {
		GetModuleFileNameEx(procHandle, modulesList[i], modulePath, pathLength);
		wchar_t *fileName = wcsrchr(modulePath, '\\') + 1;

        // case insensetive comparing routine
        // standard wcscmp() checking fails on Win10
		if (_wcsicmp(fileName, moduleName) == 0)
		{
			moduleHandle = modulesList[i];
			break;
		}
	}
	return moduleHandle;
}

size_t GetFunctionOffset(HMODULE moduleHandle, const char *funcName)
{
	uint8_t *funcAddr = (uint8_t *)GetProcAddress(moduleHandle, funcName);
	return (size_t)(funcAddr - (uint8_t*)moduleHandle);
}

bool GetModuleInfo(HANDLE procHandle, HMODULE moduleHandle, moduleinfo_t &moduleInfo)
{
	MODULEINFO targetInfo;
    const int dataSize = sizeof(targetInfo);

    if (GetModuleInformation(procHandle, moduleHandle, &targetInfo, dataSize))
    {
        moduleInfo.baseAddr         = (uint8_t*)targetInfo.lpBaseOfDll;
        moduleInfo.imageSize        = targetInfo.SizeOfImage;
        moduleInfo.entryPointAddr   = (uint8_t*)targetInfo.EntryPoint;
        return true;
    }
    else
	    return false;
}
