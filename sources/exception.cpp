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


#include "exception.h"
#include <cstdio>
#include <cstring>
#include <sstream>

CException::CException(
    std::string description,
    const char *funcName,
    const char *fileName,
    int lineNumber
)
{
    m_funcName = funcName;
    m_fileName = fileName;
    m_description = description;
    m_lineNumber = lineNumber;
    m_message.clear();
}

const std::string &CException::GetFormattedMessage()
{
    std::stringstream exMessage;
    exMessage << m_funcName << "() [" << m_fileName << ":" << m_lineNumber << "]: " << m_description << "\n";
    m_message = exMessage.str();
    return m_message;
}

const std::string &CException::GetDescription() const
{
    return m_description;
}

const std::string &CException::GetFunctionName() const
{
    return m_funcName;
}

const std::string &CException::GetFileName() const
{
    return m_fileName;
}

int CException::GetLineNumber() const
{
    return m_lineNumber;
}
