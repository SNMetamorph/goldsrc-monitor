#include "buildinfo.h"
#include "util.h"

#define SIGN_BUILD_NUMBER		"\xA1\x00\x00\x00\x00\x83\xEC\x08\x00\x33\x00\x85\xC0"
#define MASK_BUILD_NUMBER		"x????xxx?x?xx"
#define SIGN_BUILD_NUMBER_NEW	"\x55\x8B\xEC\x83\xEC\x08\xA1\x00\x00\x00\x00\x56\x33\xF6\x85\xC0\x0F\x85\x00\x00\x00\x00\x53\x33\xDB\x8B\x04\x9D"
#define MASK_BUILD_NUMBER_NEW	"xxxxxxx????xxxxxxx????xxxxxx"


int (*pfnGetBuildNumber)();
static const buildinfo_entry_t g_aBuildInfo[] = 
{
    { 
        4554, 
        {
            {FUNCTYPE_SPR_LOAD, "xxxxxxxxx????xxxx", "\x56\x8D\x44\x24\x08\x57\x50\xFF\x15\x2A\x2A\x2A\x2A\x8B\x44\x24\x10"},
            {FUNCTYPE_SPR_FRAMES, "xxxxxxx????xxxx", "\x8D\x44\x24\x04\x50\xFF\x15\x2A\x2A\x2A\x2A\x8B\x4C\x24\x08"},
            {FUNCTYPE_PRECACHE_MODEL,"xxxxxxxxxxxxxxx????", "\x53\x55\x8B\x6C\x24\x0C\x33\xDB\x56\x57\x85\xED\x75\x14\x68\x48\xEF\xA9\x04"},
            {FUNCTYPE_PRECACHE_SOUND, "xxxxxxxxxxxx????", "\x53\x8B\x5C\x24\x08\x56\x57\x85\xDB\x75\x13\x68\x38\xEC\xA9\x04"}
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

void *FindFunctionAddress(functype_t funcType, void *startAddr, void *endAddr)
{
    int currBuildNumber = GetBuildNumber();
    const int buildInfoLen = sizeof(g_aBuildInfo) / sizeof(g_aBuildInfo[0]);
    const int lastEntryIndex = buildInfoLen - 1;
    const funcdata_t *funcData = 
        &g_aBuildInfo[lastEntryIndex].functionData[funcType];

    for (int i = 0; i < lastEntryIndex; ++i)
    {
        const buildinfo_entry_t &buildInfo = g_aBuildInfo[i];
        const buildinfo_entry_t &nextBuildInfo = g_aBuildInfo[i + 1];

        // valid only if build info entries sorted ascending
        if (nextBuildInfo.number > currBuildNumber)
        {
            funcData = &buildInfo.functionData[funcType];
            break;
        }
    }

    if (!endAddr)
        endAddr = (uint8_t*)startAddr + strlen(funcData->mask);

    return FindPatternAddress(
        startAddr, 
        endAddr, 
        funcData->signature, 
        funcData->mask
    );
}

bool FindBuildNumberFunc(const moduleinfo_t &engineModule)
{
    uint8_t *moduleStartAddr = engineModule.baseAddr;
    uint8_t *moduleEndAddr = moduleStartAddr + engineModule.imageSize;

    pfnGetBuildNumber = (int(*)())FindPatternAddress(
        moduleStartAddr,
        moduleEndAddr,
    	SIGN_BUILD_NUMBER,
    	MASK_BUILD_NUMBER
    );

    if (!pfnGetBuildNumber)
    {
        pfnGetBuildNumber = (int(*)())FindPatternAddress(
            moduleStartAddr,
            moduleEndAddr,
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
