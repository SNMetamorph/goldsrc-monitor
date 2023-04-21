#pragma once
#include <stdint.h>
#include <string>

enum class InjectStatus
{
	Success,
	Failed,
	AlreadyInjected,
};

class IInjectStrategy
{
public:
	virtual ~IInjectStrategy() {};
	virtual InjectStatus Start(size_t injectDelayMsec, 
		const std::string &processName, const std::string &libraryName) = 0;
};
