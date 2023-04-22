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
