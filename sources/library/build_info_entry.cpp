#include "build_info_entry.h"

bool CBuildInfoEntry::Validate() const
{
    if (m_iClientEngfuncsOffset > 0)
        return true;

    for (int i = 0; i < FUNCTYPE_PRECACHE_MODEL; ++i) // check only client-side functions
    {
        const CMemoryPattern &pattern = m_FunctionPatterns[i];
        if (!pattern.IsInitialized()) {
            return false;
        }
    }
    return true;
}
