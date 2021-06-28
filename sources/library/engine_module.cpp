#include "engine_module.h"

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
        if (m_hModule) 
        {
            m_isSoftwareRenderer = true;
        }
        else 
        {
            m_isXashEngine = true;
            m_hModule = GetModuleHandle("xash.dll");
        }
    }
    return (m_hModule != NULL) && SetupModuleInfo();
}

bool CEngineModule::SetupModuleInfo()
{
    if (m_hModule)
    {
        if (Utils::GetModuleInfo(GetCurrentProcess(), m_hModule, m_ModuleInfo)) {
            return true;
        }
    }
    return false;
}
