#include "hooks_logger.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#endif

CHooks::Logger::~Logger()
{
}

void CHooks::Logger::log(const std::string &msg, PLH::ErrorLevel level)
{
	if (level >= m_ErrorLevel) 
	{
		switch (level) 
		{
		case PLH::ErrorLevel::INFO:
			Utils::Snprintf(m_szOutputText, "[goldsrc-monitor] INFO: %s\n", msg.c_str());
			break;
		case PLH::ErrorLevel::WARN:
			Utils::Snprintf(m_szOutputText, "[goldsrc-monitor] WARN: %s\n", msg.c_str());
			break;
		case PLH::ErrorLevel::SEV:
			Utils::Snprintf(m_szOutputText, "[goldsrc-monitor] ERROR: %s\n", msg.c_str());
			break;
		default:
			Utils::Snprintf(m_szOutputText, "[goldsrc-monitor] Unsupported error message logged: %s\n", msg.c_str());
		}
#ifdef _WIN32
		OutputDebugStringA(m_szOutputText.c_str());
#else
#pragma warning "Logging is not implemented for this platform"
#endif
	}
}

void CHooks::Logger::setLogLevel(PLH::ErrorLevel level)
{
	m_ErrorLevel = level;
}
