#include "build_info_entry.h"

bool CBuildInfoEntry::Validate() const
{
    if (m_iClientEngfuncsOffset > 0 && m_iServerEngfuncsOffset > 0)
        return true;

    for (int i = 0; i < FUNCTYPE_COUNT; ++i) 
    {
        const CMemoryPattern &pattern = m_FunctionPatterns[i];
        if (!pattern.IsInitialized()) {
            return false;
        }
    }
    return true;
}
