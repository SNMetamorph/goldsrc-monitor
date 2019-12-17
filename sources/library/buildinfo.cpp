#include "buildinfo.h"
#include "util.h"
#include "memory_defs.h"

int (*pfnGetBuildNumber)();
static const buildinfo_entry_t g_aBuildInfo[] = 
{
    { 
        4554, 
        {
            {FUNCTYPE_SPR_LOAD},
            {FUNCTYPE_SPR_FRAMES},
            {FUNCTYPE_PRECACHE_MODEL},
            {FUNCTYPE_PRECACHE_SOUND}
        }
    },
    {
        8196,
        {
            {FUNCTYPE_SPR_LOAD, "xxxxxxxxxxx????xxxxxxxxxx????", "\x55\x8B\xEC\x56\x8D\x45\x08\x57\x50\xFF\x15\x00\x00\x00\x00\x8B\x45\x08\x83\xC4\x04\x85\xC0\x0F\x84\x00\x00\x00\x00"},
            {FUNCTYPE_SPR_FRAMES, "xxxxxxxxx????xxxxx????", "\x55\x8B\xEC\x8D\x45\x08\x50\xFF\x15\x00\x00\x00\x00\x8B\x4D\x08\x51\xE8\x00\x00\x00\x00"},
            {FUNCTYPE_PRECACHE_MODEL, "xxxxxxxxxxxxxxxx????x????", "\x55\x8B\xEC\x53\x56\x57\x8B\x7D\x08\x33\xDB\x85\xFF\x75\x14\x68\x50\xDF\x97\x03\xE8\xB7\x74\xFE\xFF"},
            {FUNCTYPE_PRECACHE_SOUND, "xxxxxxxxxxxxx????x????", "\x55\x8B\xEC\x56\x57\x8B\x7D\x08\x85\xFF\x75\x13\x68\x3C\xDC\x97\x03\xE8\x6A\x7C\xFE\xFF"}
        }
    }
};

void *FindFunctionAddress(functype_t funcType, void *startAddr, int scanLen)
{
    int currBuildNumber = GetBuildNumber();
    const int buildInfoLen = sizeof(g_aBuildInfo) / sizeof(g_aBuildInfo[0]);
    const int lastEntryIndex = buildInfoLen - 1;

    for (int i = 0; i < lastEntryIndex; ++i)
    {
        const buildinfo_entry_t &buildInfo = g_aBuildInfo[i];
        const buildinfo_entry_t &nextBuildInfo = g_aBuildInfo[i + 1];

        // valid only if build info entries sorted ascending
        if (nextBuildInfo.number > currBuildNumber)
        {
            const funcdata_t &funcData = buildInfo.functionData[funcType];
            return FindPatternAddress(
                startAddr, scanLen, funcData.signature, funcData.mask);
        }
    }

    const funcdata_t &funcData = 
        g_aBuildInfo[lastEntryIndex].functionData[funcType];
    return FindPatternAddress(
        startAddr, 
        scanLen, 
        funcData.signature, 
        funcData.mask
    );
}

bool FindBuildNumberFunc(const moduleinfo_t &engineModule)
{
    pfnGetBuildNumber = (int(*)())FindPatternAddress(
        engineModule.baseAddr,
        engineModule.imageSize,
    	SIGN_BUILD_NUMBER,
    	MASK_BUILD_NUMBER
    );

    if (!pfnGetBuildNumber)
    {
        pfnGetBuildNumber = (int(*)())FindPatternAddress(
            engineModule.baseAddr,
            engineModule.imageSize,
            SIGN_BUILD_NUMBER_NEW,
            MASK_BUILD_NUMBER_NEW
        );
    }
    
    if (pfnGetBuildNumber && pfnGetBuildNumber() > 0)
        return true;
    else
        return false;
}

int GetBuildNumber()
{
    if (pfnGetBuildNumber)
        return pfnGetBuildNumber();
    else
        return 0;
}
