/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

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
