#pragma once
#include "hooks.h"
#include <polyhook2/Enums.hpp>
#include <polyhook2/ErrorLog.hpp>
#include <string>

class CHooks::Logger : public PLH::Logger 
{
public:
	~Logger() override;
	void log(const std::string &msg, PLH::ErrorLevel level) override;
	void setLogLevel(PLH::ErrorLevel level);

private:
	std::string m_szOutputText;
	PLH::ErrorLevel m_ErrorLevel = PLH::ErrorLevel::NONE;
};
