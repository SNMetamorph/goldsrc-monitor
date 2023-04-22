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

