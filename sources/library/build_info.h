#pragma once
#include "module_info.h"
#include <memory>

class CBuildInfo
{
public:
    class Entry;
    enum class FunctionType
    {
        // client-side
        SPR_Load,
        SPR_Frames,
        // server-side
        PrecacheModel,
        PrecacheSound,
        Count, // keep this last
    };

    CBuildInfo();
    ~CBuildInfo();
    void Initialize(const ModuleInfo &engineModule);
    void *FindFunctionAddress(FunctionType funcType, void *startAddr, void *endAddr = nullptr) const;
    const Entry &GetInfoEntry() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};
