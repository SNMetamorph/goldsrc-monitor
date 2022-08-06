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
