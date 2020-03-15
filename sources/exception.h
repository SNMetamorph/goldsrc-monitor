#pragma once

// Universal macros
#define EXCEPT(message) (throw CException(message, __FUNCTION__, __FILE__, __LINE__))

class CException
{
public:
    CException(
        const char *description,
        const char *funcName,
        const char *sourceFilePath,
        int lineNumber
    );

    const char *GetFormattedMessage();
    const char *GetDescription() const;
    const char *GetFunctionName() const;
    const char *GetFileName() const;
    int			GetLineNumber() const;

    char m_szMessageBuffer[256];

private:
    int m_iLineNumber;
    const char *m_szDescription;
    const char *m_szFuncName;
    const char *m_szFilePath;
};

