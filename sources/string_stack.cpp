#include "string_stack.h"
#include <string>
#include <stdarg.h>

CStringStack::CStringStack(char *stringArray, int stringLen, int maxStringCount)
{
    m_iStackIndex   = 0;
    m_iStringLen    = stringLen;
    m_iArraySize    = maxStringCount;
    m_aStringArray  = stringArray;
}

bool CStringStack::Push(const char *str)
{
    char *stringAddr;
    if (m_iStackIndex < m_iArraySize)
    {
        stringAddr = &m_aStringArray[m_iStackIndex * m_iStringLen];
        strncpy(stringAddr, str, m_iStringLen);
        ++m_iStackIndex;
        return true;
    }
    return false;
}

bool CStringStack::PushPrintf(const char *format, ...)
{
    va_list args;
    char *stringAddr;

    if (m_iStackIndex < m_iArraySize)
    {
        stringAddr = &m_aStringArray[m_iStackIndex * m_iStringLen];
        va_start(args, format);
        vsnprintf(stringAddr, m_iStringLen, format, args);
        va_end(args);

        ++m_iStackIndex;
        return true;
    }
    return false;
}

void CStringStack::Pop()
{
    if (m_iStackIndex > 0)
        --m_iStackIndex;
}

void CStringStack::Clear()
{
    m_iStackIndex = 0;
}

const char *CStringStack::StringAt(int index)
{
    if (index >= 0 && index < m_iArraySize)
        return &m_aStringArray[index * m_iStringLen];
    else
        return nullptr;
}
