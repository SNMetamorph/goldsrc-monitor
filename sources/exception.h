#pragma once
#include <string>
#include <string_view>

#define EXCEPT(msg) (throw CException((msg), __FUNCTION__, CException::TruncFilePath(__FILE__), __LINE__))

class CException
{
public:
    CException(
        std::string description,
        const char *funcName,
        const char *fileName,
        int lineNumber
    );

    const std::string &CException::GetFormattedMessage();
    const std::string &GetDescription() const;
    const std::string &GetFunctionName() const;
    const std::string &GetFileName() const;
    int GetLineNumber() const;

    static constexpr const char *TruncFilePath(const char *filePath)
    {
        std::string_view sv = filePath;
        return filePath + sv.find_last_of("/\\") + 1;
    };

private:
    int m_iLineNumber;
    std::string m_szDescription;
    std::string m_szFuncName;
    std::string m_szFileName;
    std::string m_szMessage;
};

