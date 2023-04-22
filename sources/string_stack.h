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
#include <stdio.h>
#include <vector>

class CStringStack
{
public:
    CStringStack(int stringLen);

    void Pop();
    void Clear();
    void Push(const char *str);
    void PushPrintf(const char *format, ...);
    
    inline int GetStringLength() const      { return m_iStringLen;  };
    inline int GetStringCount() const       { return m_iStackIndex; };
    inline int GetBufferSize() const        { return m_StringBuffer.capacity(); };
    const char *StringAt(int index) const;

private:
    void AllocString();

    int m_iStackIndex = 0;
    int m_iStringLen = 0;
    std::vector<char> m_StringBuffer;
};

