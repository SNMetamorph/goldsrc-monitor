/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

class ProcessHandle
{
public:
#ifdef _WIN32
	ProcessHandle(HANDLE handle);
	operator HANDLE() const;
#endif
	bool Valid() const;

private:
	int64_t m_iHandle = -1;
};

class ModuleHandle
{
public:
#ifdef _WIN32
	ModuleHandle(HMODULE handle);
	operator HMODULE() const;
#endif
	ModuleHandle() {};
	bool Valid() const;

private:
	int64_t m_iHandle = -1;
};

namespace SysUtils
{
	struct ModuleInfo
	{
		size_t imageSize;
		uint8_t *baseAddress;
		uint8_t *entryPointAddress;
	};

	void Sleep(size_t timeMsec);
	float GetCurrentSysTime();
	void InitCurrentLibraryHandle(ModuleHandle handle);
	ModuleHandle GetCurrentLibraryHandle();
	ModuleHandle GetCurrentProcessModule();
	ProcessHandle GetCurrentProcessHandle();
	bool GetModuleDirectory(ModuleHandle moduleHandle, std::string &workingDir);
	bool GetModuleFilename(ModuleHandle moduleHandle, std::string &fileName);
	bool GetModuleInfo(ProcessHandle procHandle, ModuleHandle moduleHandle, ModuleInfo &moduleInfo);
	ModuleHandle FindModuleByExport(ProcessHandle procHandle, const char *exportName);
	ModuleHandle FindModuleInProcess(ProcessHandle procHandle, const std::string &moduleName);
	void FindProcessIdByName(const char *processName, std::vector<int32_t> &processIds);
	void *GetModuleFunction(ModuleHandle moduleHandle, const char *funcName);
}
