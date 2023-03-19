#include "hooks_impl.h"

CFunctionHook<CHooks::Impl::pfnRedraw_t> CHooks::Impl::m_hookRedraw;
CFunctionHook<CHooks::Impl::pfnPlayerMove_t> CHooks::Impl::m_hookPlayerMove;
CFunctionHook<CHooks::Impl::pfnKeyEvent_t> CHooks::Impl::m_hookKeyEvent;
CFunctionHook<CHooks::Impl::pfnDrawTriangles_t> CHooks::Impl::m_hookDrawTriangles;
CFunctionHook<CHooks::Impl::pfnIsThirdPerson_t> CHooks::Impl::m_hookIsThirdPerson;
CFunctionHook<CHooks::Impl::pfnCameraOffset_t> CHooks::Impl::m_hookCameraOffset;
CFunctionHook<CHooks::Impl::pfnVidInit_t> CHooks::Impl::m_hookVidInit;

void CHooks::Impl::InitializeLogger()
{
    m_pLogger = std::make_shared<PLH::ErrorLog>();
    m_pLogger->setLogLevel(PLH::ErrorLevel::SEV);
    PLH::Log::registerLogger(m_pLogger);
}

void CHooks::Impl::WriteLogs(std::string &errorLog)
{
    auto &logger = *m_pLogger;
    errorLog.clear();
    for (PLH::Error error = logger.pop(); !error.msg.empty(); error = logger.pop()) {
        errorLog += error.msg;
    }
}

void CHooks::Impl::RevertHooks()
{
    m_hookRedraw.Unhook();
    m_hookPlayerMove.Unhook();
    m_hookKeyEvent.Unhook();
    m_hookDrawTriangles.Unhook();
    m_hookIsThirdPerson.Unhook();
    m_hookCameraOffset.Unhook();
    m_hookVidInit.Unhook();
}
