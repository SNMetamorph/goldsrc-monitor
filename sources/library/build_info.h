#pragma once
#include "build_info_entry.h"
#include "module_info.h"
#include <memory>

class CBuildInfo
{
public:
    CBuildInfo();
    ~CBuildInfo();
    void Initialize(const ModuleInfo &engineModule);
    void *FindFunctionAddress(FunctionType funcType, void *startAddr, void *endAddr = nullptr) const;
    const CBuildInfoEntry &GetInfoEntry() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};
