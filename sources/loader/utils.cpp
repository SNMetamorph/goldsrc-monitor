#include "utils.h"

namespace fs = std::filesystem;

bool Utils::FindLibraryAbsolutePath(const std::string &libraryName, std::filesystem::path &libPath)
{
    fs::path libraryRelativePath = libraryName;
    if (fs::exists(libraryRelativePath)) {
        libPath = fs::absolute(libraryRelativePath);
        return true;
    }
    return false;
}

size_t Utils::GetFunctionOffset(ModuleHandle moduleHandle, const char *funcName)
{
    SysUtils::ModuleInfo moduleInfo;
    SysUtils::GetModuleInfo(SysUtils::GetCurrentProcessHandle(), moduleHandle, moduleInfo);
    uint8_t *funcAddr = reinterpret_cast<uint8_t*>(SysUtils::GetModuleFunction(moduleHandle, funcName));
    return (size_t)(funcAddr - moduleInfo.baseAddress);
}
