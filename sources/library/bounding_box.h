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
#include "hlsdk.h"

class CBoundingBox
{
public:
    CBoundingBox() {};
    CBoundingBox(const vec3_t &size);
    CBoundingBox(const vec3_t &vecMins, const vec3_t &vecMaxs);

    double GetSurfaceArea() const;
    vec3_t GetCenterPoint() const;
    void SetCenterToPoint(const vec3_t &point);
    CBoundingBox GetUnion(const CBoundingBox &operand) const;
    void CombineWith(const CBoundingBox &operand);
    void ExpandToPoint(const vec3_t &point);
    bool Contains(const CBoundingBox &operand) const;
    bool ContainsPoint(const vec3_t &point) const;

    inline const vec3_t &GetSize() const { return m_vecSize; };
    inline const vec3_t &GetMins() const { return m_vecMins; };
    inline const vec3_t &GetMaxs() const { return m_vecMaxs; };

private:
    void Initialize(const vec3_t &vecMins, const vec3_t &vecMaxs);

    vec3_t m_vecMins = vec3_t(0, 0, 0);
    vec3_t m_vecMaxs = vec3_t(0, 0, 0);
    vec3_t m_vecSize = vec3_t(0, 0, 0);
};
