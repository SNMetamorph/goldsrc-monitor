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
#include <stdint.h>
#include <vector>
#include <string>

class CMemoryPattern
{
public:
    CMemoryPattern() {};
    CMemoryPattern(const std::string &pattern);
    CMemoryPattern(const char *pattern, int byteCount, uint8_t wildmark = 0xCC);
    void AddByte(uint8_t value, bool shouldCheck = true);
    void InitFromBytes(uint8_t *pattern, int byteCount, uint8_t wildmark = 0xCC);
    void InitFromString(const std::string &pattern);
    bool IsInitialized() const;

    int GetLength() const { return m_signature.size(); };
    uint8_t GetByteAt(int offset) const { return m_signature[offset]; };
    bool ShouldCheckByteAt(int offset) const { return m_mask[offset]; };
    // To provide an optimal way to finding pattern address
    const int *GetMaskAddress() const { return m_mask.data(); };
    const uint8_t *GetSignatureAddress() const { return m_signature.data(); };
    
private:
    void ReserveElements(size_t elemCount);
    void Reset();

    std::vector<int> m_mask;
    std::vector<uint8_t> m_signature;
};
