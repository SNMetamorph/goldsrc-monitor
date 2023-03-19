#include "build_info.h"
#include "build_info_entry.h"

bool CBuildInfo::Entry::Validate() const
{
    if (m_iClientEngfuncsOffset > 0) {
        return true;
    }

    for (size_t i = 0; i < static_cast<size_t>(FunctionType::PrecacheModel); ++i) // check only client-side functions
    {
        const CMemoryPattern &pattern = m_FunctionPatterns[i];
        if (!pattern.IsInitialized()) {
            return false;
        }
    }
    return true;
}
