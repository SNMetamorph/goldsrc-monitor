#pragma once
#include "sys_utils.h"
#include <windows.h>
#include <stdint.h>

class CEngineModule 
{
public:
    static CEngineModule& GetInstance();

    bool FindHandle();
    bool GetFunctionsFromAPI(uint8_t **pfnSPR_Load, uint8_t **pfnSPR_Frames);
    inline bool IsXashEngine() const        { return m_isXashEngine; }
    inline bool IsSoftwareRenderer() const  { return m_isSoftwareRenderer; };
    inline ModuleHandle GetHandle() const   { return m_hModule; };
    inline uint8_t* GetAddress() const      { return m_ModuleInfo.baseAddress; };
    inline size_t GetSize() const           { return m_ModuleInfo.imageSize; }

private:
    CEngineModule() {};
    CEngineModule(const CEngineModule&) = delete;
    CEngineModule& operator=(const CEngineModule&) = delete;
    bool SetupModuleInfo();

    ModuleHandle m_hModule = NULL;
    SysUtils::ModuleInfo m_ModuleInfo;
    bool m_isXashEngine = false;
    bool m_isSoftwareRenderer = false;
};
extern CEngineModule& g_EngineModule;
