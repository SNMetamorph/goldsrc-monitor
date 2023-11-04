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

#include "bounding_box.h"
#include <algorithm>

CBoundingBox::CBoundingBox(const vec3_t &size)
{
    Initialize(-size / 2, size / 2);
}

CBoundingBox::CBoundingBox(const vec3_t &vecMins, const vec3_t &vecMaxs)
{
    Initialize(vecMins, vecMaxs);
}

vec3_t CBoundingBox::GetCenterPoint() const
{
    return m_mins + m_size / 2;
}

void CBoundingBox::SetCenterToPoint(const vec3_t &point)
{
    Initialize(point - m_size / 2, point + m_size / 2);
}

CBoundingBox CBoundingBox::GetUnion(const CBoundingBox &operand) const
{
    CBoundingBox currentBoxCopy(m_mins, m_maxs);
    currentBoxCopy.CombineWith(operand);
    return currentBoxCopy;
}

void CBoundingBox::CombineWith(const CBoundingBox &operand)
{
    vec3_t unionMins;
    vec3_t unionMaxs;
    const vec3_t &currMins = m_mins;
    const vec3_t &currMaxs = m_maxs;
    const vec3_t &operandMins = operand.GetMins();
    const vec3_t &operandMaxs = operand.GetMaxs();

    unionMins.x = std::min(currMins.x, operandMins.x);
    unionMins.y = std::min(currMins.y, operandMins.y);
    unionMins.z = std::min(currMins.z, operandMins.z);

    unionMaxs.x = std::max(currMaxs.x, operandMaxs.x);
    unionMaxs.y = std::max(currMaxs.y, operandMaxs.y);
    unionMaxs.z = std::max(currMaxs.z, operandMaxs.z);

    Initialize(unionMins, unionMaxs);
}

void CBoundingBox::ExpandToPoint(const vec3_t &point)
{
    if (point.x < m_mins.x)
        m_mins.x = point.x;
    if (point.y < m_mins.y)
        m_mins.y = point.y;
    if (point.z < m_mins.z)
        m_mins.z = point.z;

    if (point.x > m_maxs.x)
        m_maxs.x = point.x;
    if (point.y > m_maxs.y)
        m_maxs.y = point.y;
    if (point.z > m_maxs.z)
        m_maxs.z = point.z;

    Initialize(m_mins, m_maxs);
}

bool CBoundingBox::Contains(const CBoundingBox &operand) const
{
    const vec3_t &mins = operand.GetMins();
    const vec3_t &maxs = operand.GetMaxs();

    if (mins.x < m_mins.x || mins.x > m_maxs.x)
        return false;
    if (mins.y < m_mins.y || mins.y > m_maxs.y)
        return false;
    if (mins.z < m_mins.z || mins.z > m_maxs.z)
        return false;

    if (maxs.x < m_mins.x || maxs.x > m_maxs.x)
        return false;
    if (maxs.y < m_mins.y || maxs.y > m_maxs.y)
        return false;
    if (maxs.z < m_mins.z || maxs.z > m_maxs.z)
        return false;

    return true;
}

bool CBoundingBox::ContainsPoint(const vec3_t &point) const
{
    if (point.x < m_mins.x || point.x > m_maxs.x)
        return false;
    if (point.y < m_mins.y || point.y > m_maxs.y)
        return false;
    if (point.z < m_mins.z || point.z > m_maxs.z)
        return false;
    return true;
}

double CBoundingBox::GetSurfaceArea() const
{
    return 2.0 * (m_size.x * m_size.y + m_size.y * m_size.z + m_size.x * m_size.z);
}

void CBoundingBox::Initialize(const vec3_t &vecMins, const vec3_t &vecMaxs)
{
    m_mins = vecMins;
    m_maxs = vecMaxs;
    m_size = vecMaxs - vecMins;
}
