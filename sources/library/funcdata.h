#pragma once

enum functype_t
{
    FUNCTYPE_SPR_LOAD,
    FUNCTYPE_SPR_FRAMES,
    FUNCTYPE_PRECACHE_MODEL,
    FUNCTYPE_PRECACHE_SOUND,
    FUNCTYPE_COUNT, // keep this last
};

struct funcdata_t
{
    functype_t type;
    const char *mask;
    const char *signature;
};
