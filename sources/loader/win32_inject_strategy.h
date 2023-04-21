#pragma once
#include "inject_strategy.h"
#include <memory>

class CWin32InjectStrategy : public IInjectStrategy
{
public:
	CWin32InjectStrategy();
	~CWin32InjectStrategy() override;
	InjectStatus Start(size_t injectDelayMsec, 
		const std::string &processName, const std::string &libraryName) override;

	class Impl;
private:
	std::unique_ptr<Impl> m_pImpl;
};
