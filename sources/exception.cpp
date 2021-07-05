#include <cstdio>
#include <cstring>
#include <sstream>
#include "exception.h"

CException::CException(
    std::string description,
    const char *funcName,
    const char *sourceFilePath,
    int lineNumber
)
{
    m_szFuncName = funcName;
    m_szFilePath = sourceFilePath;
    m_szDescription = description;
    m_iLineNumber = lineNumber;
    m_szMessage.clear();
}

const std::string &CException::GetFormattedMessage()
{
    std::stringstream exMessage;
    exMessage << m_szFuncName << "() [" << GetFileName() << ":" << m_iLineNumber << "]: " << m_szDescription << "\n";
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

const char *CException::GetFileName() const
{
    return strrchr(m_szFilePath.c_str(), '\\') + 1;
}

int CException::GetLineNumber() const
{
    return m_iLineNumber;
}
