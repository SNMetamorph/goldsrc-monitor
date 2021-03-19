#include "engine_module.h"
#include "utils.h"

CEngineModule& g_EngineModule = CEngineModule::GetInstance();

CEngineModule& CEngineModule::GetInstance()
{
    static CEngineModule instance;
    return instance;
}

bool CEngineModule::FindHandle()
{
    m_hModule = GetModuleHandle("hw.dll");
    if (!m_hModule)
    {
        m_hModule = GetModuleHandle("sw.dll");
        m_isSoftwareRenderer = true;
        if (!m_hModule)
            m_hModule = GetModuleHandle("swds.dll");
    }
    return (m_hModule != NULL) && SetupModuleInfo();
}

bool CEngineModule::SetupModuleInfo()
{
    if (m_hModule)
    {
        moduleinfo_t moduleInfo;
        if (Utils::GetModuleInfo(GetCurrentProcess(), m_hModule, moduleInfo))
        {
            m_iSize = moduleInfo.imageSize;
            m_pAddress = moduleInfo.baseAddr;
            return true;
        }
    }
    return false;
}
