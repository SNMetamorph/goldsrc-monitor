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

#include "memory_pattern.h"
#include "exception.h"
#include <exception>
#include <sstream>
#include <iterator>

CMemoryPattern::CMemoryPattern(const std::string &pattern)
{
    InitFromString(pattern);
}

CMemoryPattern::CMemoryPattern(const char *pattern, int byteCount, uint8_t wildmark)
{
    InitFromBytes((uint8_t*)pattern, byteCount, wildmark);
}

void CMemoryPattern::ReserveElements(size_t elemCount)
{
    m_mask.reserve(elemCount);
    m_signature.reserve(elemCount);
}

void CMemoryPattern::Reset()
{
    m_mask.clear();
    m_signature.clear();
}

void CMemoryPattern::AddByte(uint8_t value, bool shouldCheck)
{
    m_mask.push_back(shouldCheck);
    m_signature.push_back(value);
}

void CMemoryPattern::InitFromBytes(uint8_t *pattern, int byteCount, uint8_t wildmark)
{
    Reset();
    ReserveElements(byteCount);

    for (int i = 0; i < byteCount; ++i)
    {
        uint8_t *currentByte = pattern + i;
        if (currentByte[0] != wildmark)
        {
            m_signature.push_back(currentByte[0]);
            m_mask.push_back(true);
        }
        else 
        {
            m_signature.push_back(0x0);
            m_mask.push_back(false);
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
    ReserveElements(128); // pre-allocate some elements

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

        m_mask.shrink_to_fit();
        m_signature.shrink_to_fit();
    }
    catch (std::exception &ex)
    {
        EXCEPT(std::string("memory pattern parsing error: ") + ex.what());
    }
}

bool CMemoryPattern::IsInitialized() const
{
    if (m_mask.size() < 1 || m_signature.size() < 1) {
        return false;
    }
    return true;
}
