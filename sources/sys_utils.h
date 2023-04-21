#pragma once
#include <string>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
using ProcessHandle = HANDLE;
using ModuleHandle = HMODULE;
#else
using ProcessHandle = int;
using ModuleHandle = int;
#endif

namespace SysUtils
{
	struct ModuleInfo
	{
		size_t imageSize;
		uint8_t *baseAddress;
		uint8_t *entryPointAddress;
	};

	void InitCurrentLibraryHandle(ModuleHandle handle);
	ModuleHandle GetCurrentLibraryHandle();
	ModuleHandle GetCurrentProcessModule();
	ProcessHandle GetCurrentProcessHandle();
	bool GetModuleDirectory(ModuleHandle moduleHandle, std::string &workingDir);
	bool GetModuleFilename(ModuleHandle moduleHandle, std::string &fileName);
	bool GetModuleInfo(ProcessHandle procHandle, ModuleHandle moduleHandle, ModuleInfo &moduleInfo);
	ModuleHandle FindModuleByExport(ProcessHandle procHandle, const char *exportName);
}
