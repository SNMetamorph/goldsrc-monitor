#include "string_stack.h"
#include <string>
#include <stdarg.h>

CStringStack::CStringStack(int stringLen)
{
    m_iStackIndex   = 0;
    m_iStringLen    = stringLen;
    m_StringBuffer.clear();
}

void CStringStack::Push(const char *str)
{
    AllocString();
    char *stringAddr = &m_StringBuffer[m_iStackIndex * m_iStringLen];
    strncpy(stringAddr, str, m_iStringLen);
    ++m_iStackIndex;
}

void CStringStack::PushPrintf(const char *format, ...)
{
    va_list args;
    AllocString();
    char *stringAddr = &m_StringBuffer[m_iStackIndex * m_iStringLen];
    va_start(args, format);
    vsnprintf(stringAddr, m_iStringLen, format, args);
    va_end(args);
    ++m_iStackIndex;
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

const char *CStringStack::StringAt(int index) const
{
    if (index >= 0 && index < m_iStackIndex)
        return &m_StringBuffer[index * m_iStringLen];
    else
        return nullptr;
}

void CStringStack::AllocString()
{
    int currentSize = m_StringBuffer.size();
    int desiredSize = m_iStackIndex * m_iStringLen + m_iStringLen;
    if (currentSize < desiredSize)
        m_StringBuffer.resize(desiredSize);
}
