#include "engine_module.h"
#include "hlsdk.h"

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

bool CEngineModule::GetFunctionsFromAPI(uint8_t **pfnSPR_Load, uint8_t **pfnSPR_Frames)
{
    if (m_isXashEngine)
    {
        pfnEngSrc_pfnSPR_Load_t pfnFunc1 = (pfnEngSrc_pfnSPR_Load_t)GetProcAddress(m_hModule, "pfnSPR_Load");
        if (pfnFunc1)
        {
            pfnEngSrc_pfnSPR_Frames_t pfnFunc2 = (pfnEngSrc_pfnSPR_Frames_t)GetProcAddress(m_hModule, "pfnSPR_Frames");
            if (pfnFunc2)
            {
                *pfnSPR_Load = reinterpret_cast<uint8*>(pfnFunc1);
                *pfnSPR_Frames = reinterpret_cast<uint8 *>(pfnFunc2);
                return true;
            }
        }
    }
    return false;
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
