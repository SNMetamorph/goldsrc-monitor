#pragma once
#include <string>

#define EXCEPT(msg) (throw CException((msg), __FUNCTION__, __FILE__, __LINE__))

class CException
{
public:
    CException(
        std::string description,
        const char *funcName,
        const char *sourceFilePath,
        int lineNumber
    );

    const std::string &CException::GetFormattedMessage();
    const std::string &GetDescription() const;
    const std::string &GetFunctionName() const;
    const char *GetFileName() const;
    int GetLineNumber() const;

private:
    int m_iLineNumber;
    std::string m_szDescription;
    std::string m_szFuncName;
    std::string m_szFilePath;
    std::string m_szMessage;
};

