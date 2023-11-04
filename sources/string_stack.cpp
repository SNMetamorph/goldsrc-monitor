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

#include "string_stack.h"
#include <string>
#include <stdarg.h>

CStringStack::CStringStack(int stringLen)
{
    m_stackIndex   = 0;
    m_stringLen    = stringLen;
    m_stringBuffer.clear();
}

void CStringStack::Push(const char *str)
{
    AllocString();
    char *stringAddr = &m_stringBuffer[m_stackIndex * m_stringLen];
    strncpy(stringAddr, str, m_stringLen);
    ++m_stackIndex;
}

void CStringStack::PushPrintf(const char *format, ...)
{
    va_list args;
    AllocString();
    char *stringAddr = &m_stringBuffer[m_stackIndex * m_stringLen];
    va_start(args, format);
    vsnprintf(stringAddr, m_stringLen, format, args);
    va_end(args);
    ++m_stackIndex;
}

void CStringStack::Pop()
{
    if (m_stackIndex > 0)
        --m_stackIndex;
}

void CStringStack::Clear()
{
    m_stackIndex = 0;
}

const char *CStringStack::StringAt(int index) const
{
    if (index >= 0 && index < m_stackIndex)
        return &m_stringBuffer[index * m_stringLen];
    else
        return nullptr;
}

void CStringStack::AllocString()
{
    int currentSize = m_stringBuffer.size();
    int desiredSize = m_stackIndex * m_stringLen + m_stringLen;
    if (currentSize < desiredSize)
        m_stringBuffer.resize(desiredSize);
}
