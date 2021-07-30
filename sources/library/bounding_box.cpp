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
    return m_vecMins + m_vecSize / 2;
}

void CBoundingBox::SetCenterToPoint(const vec3_t &point)
{
    Initialize(point - m_vecSize / 2, point + m_vecSize / 2);
}

CBoundingBox CBoundingBox::GetUnion(const CBoundingBox &operand) const
{
    CBoundingBox currentBoxCopy(m_vecMins, m_vecMaxs);
    currentBoxCopy.CombineWith(operand);
    return currentBoxCopy;
}

void CBoundingBox::CombineWith(const CBoundingBox &operand)
{
    vec3_t unionMins;
    vec3_t unionMaxs;
    const vec3_t &currMins = m_vecMins;
    const vec3_t &currMaxs = m_vecMaxs;
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

bool CBoundingBox::Contains(const CBoundingBox &operand) const
{
    const vec3_t &mins = operand.GetMins();
    const vec3_t &maxs = operand.GetMaxs();

    if (mins.x < m_vecMins.x || mins.x > m_vecMaxs.x)
        return false;
    if (mins.y < m_vecMins.y || mins.y > m_vecMaxs.y)
        return false;
    if (mins.z < m_vecMins.z || mins.z > m_vecMaxs.z)
        return false;

    if (maxs.x < m_vecMins.x || maxs.x > m_vecMaxs.x)
        return false;
    if (maxs.y < m_vecMins.y || maxs.y > m_vecMaxs.y)
        return false;
    if (maxs.z < m_vecMins.z || maxs.z > m_vecMaxs.z)
        return false;

    return true;
}

bool CBoundingBox::ContainsPoint(const vec3_t &point) const
{
    if (point.x < m_vecMins.x || point.x > m_vecMaxs.x)
        return false;
    if (point.y < m_vecMins.y || point.y > m_vecMaxs.y)
        return false;
    if (point.z < m_vecMins.z || point.z > m_vecMaxs.z)
        return false;
    return true;
}

double CBoundingBox::GetSurfaceArea() const
{
    return 2.0 * (m_vecSize.x * m_vecSize.y + m_vecSize.y * m_vecSize.z + m_vecSize.x * m_vecSize.z);
}

void CBoundingBox::Initialize(const vec3_t &vecMins, const vec3_t &vecMaxs)
{
    m_vecMins = vecMins;
    m_vecMaxs = vecMaxs;
    m_vecSize = vecMaxs - vecMins;
}
