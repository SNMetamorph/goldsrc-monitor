#include "bounding_box.h"

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
    return m_vecMins + m_vecMaxs;
}

void CBoundingBox::SetCenterToPoint(const vec3_t &point)
{
    Initialize(m_vecMins + point, m_vecMaxs + point);
}

float CBoundingBox::GetSurfaceArea() const
{
    return 2.f * (m_vecSize.x * m_vecSize.y + m_vecSize.y * m_vecSize.z + m_vecSize.x * m_vecSize.z);
}

void CBoundingBox::Initialize(const vec3_t &vecMins, const vec3_t &vecMaxs)
{
    m_vecMins = vecMins;
    m_vecMaxs = vecMaxs;
    m_vecSize = vecMaxs - vecMins;
}
