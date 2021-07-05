#include "memory_pattern.h"
#include "exception.h"
#include <exception>
#include <sstream>

CMemoryPattern::CMemoryPattern(const std::string &pattern)
{
    Allocate();
    InitFromString(pattern);
}

CMemoryPattern::CMemoryPattern(const char *pattern, int byteCount, uint8_t wildmark)
{
    Allocate();
    InitFromBytes((uint8_t*)pattern, byteCount, wildmark);
}

void CMemoryPattern::Allocate()
{
    // to avoid extra allocations
    const int approxSize = 32;
    m_Mask.reserve(approxSize);
    m_Signature.reserve(approxSize);
}

void CMemoryPattern::Reset()
{
    m_Mask.clear();
    m_Signature.clear();
}

void CMemoryPattern::AddByte(uint8_t value, bool shouldCheck)
{
    m_Mask.push_back(shouldCheck);
    m_Signature.push_back(value);
}

void CMemoryPattern::InitFromBytes(uint8_t *pattern, int byteCount, uint8_t wildmark)
{
    Reset();
    for (int i = 0; i < byteCount; ++i)
    {
        uint8_t *currentByte = pattern + i;
        if (currentByte[0] != wildmark)
        {
            m_Signature.push_back(currentByte[0]);
            m_Mask.push_back(true);
        }
        else 
        {
            m_Signature.push_back(0x0);
            m_Mask.push_back(false);
        }
    }
}

// Parses string in format like "FF A5 B1 ?? ?? AA C5"
// Not so good at terms of perfomance but at least isn't complicated
void CMemoryPattern::InitFromString(const std::string &pattern)
{
    std::stringstream pattern_stream(pattern);
    std::vector<std::string> tokens(
        std::istream_iterator<std::string, char>(pattern_stream),
        {}
    );
    Reset();
    try {
        for (const std::string &token : tokens)
        {
            if (token.compare("??") != 0) {
                AddByte(static_cast<uint8_t>(std::stoul(token, 0, 16)));
            }
            else {
                AddByte(0x0, false);
            }
        }
    }
    catch (std::exception &ex)
    {
        EXCEPT(std::string("memory pattern parsing error: ") + ex.what());
    }
}

bool CMemoryPattern::IsInitialized() const
{
    if (m_Mask.size() < 1 || m_Signature.size() < 1) {
        return false;
    }
    return true;
}
