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
    m_szFuncName = funcName;
    m_szFileName = fileName;
    m_szDescription = description;
    m_iLineNumber = lineNumber;
    m_szMessage.clear();
}

const std::string &CException::GetFormattedMessage()
{
    std::stringstream exMessage;
    exMessage << m_szFuncName << "() [" << m_szFileName << ":" << m_iLineNumber << "]: " << m_szDescription << "\n";
    m_szMessage = exMessage.str();
    return m_szMessage;
}

const std::string &CException::GetDescription() const
{
    return m_szDescription;
}

const std::string &CException::GetFunctionName() const
{
    return m_szFuncName;
}

const std::string &CException::GetFileName() const
{
    return m_szFileName;
}

int CException::GetLineNumber() const
{
    return m_iLineNumber;
}
