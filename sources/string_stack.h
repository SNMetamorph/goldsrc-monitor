#pragma once
#include <stdio.h>

class CStringStack
{
public:
    CStringStack(char *stringArray, int stringLen, int maxStringCount);

    void Pop();
    void Clear();
    bool Push(const char *str);
    bool PushPrintf(const char *format, ...);
    
    inline int GetStringLength() const      { return m_iStringLen;  };
    inline int GetStringCount() const       { return m_iStackIndex; };
    inline int GetStringMaxCount() const    { return m_iArraySize;  };
    const char *StringAt(int index) const;

private:
    int m_iStackIndex;
    int m_iStringLen;
    int m_iArraySize;
    char *m_aStringArray;
};

