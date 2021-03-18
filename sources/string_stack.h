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

