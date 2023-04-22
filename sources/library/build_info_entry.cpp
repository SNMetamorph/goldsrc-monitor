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
