#include "buildinfo.h"
#include "utils.h"

static int (*g_pfnGetBuildNumber)();
static int g_iBuildNumber;
static char *g_szMonNames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char g_iMonDaysCount[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
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


const char *FindDateString(uint8_t *startAddr, int maxLen)
{
    const int dateStringLen = 11;
    maxLen -= dateStringLen;

    for (int i = 0; i < maxLen; ++i)
    { 
        int index = 0;
        const char *text = reinterpret_cast<const char*>(startAddr + i);
        if (Utils::IsSymbolAlpha(text[index++]) &&
            Utils::IsSymbolAlpha(text[index++]) &&
            Utils::IsSymbolAlpha(text[index++]) &&
            Utils::IsSymbolSpace(text[index++]))
        {
            if (Utils::IsSymbolDigit(text[index]) || Utils::IsSymbolSpace(text[index]))
            {
                ++index;
                if (Utils::IsSymbolDigit(text[index++]) &&
                    Utils::IsSymbolSpace(text[index++]) &&
                    Utils::IsSymbolDigit(text[index++]) &&
                    Utils::IsSymbolDigit(text[index++]) &&
                    Utils::IsSymbolDigit(text[index++]))
                        return text;
            }
        }
    }
    return nullptr;
}

int GetBuildNumberByDate(const char *date)
{
    int m = 0, d = 0, y = 0;
    static int b = 0;

    if (b != 0) 
        return b;

    for (m = 0; m < 11; m++)
    {
        if (!strnicmp(&date[0], g_szMonNames[m], 3))
            break;
        d += g_iMonDaysCount[m];
    }

    d += atoi(&date[4]) - 1;
    y = atoi(&date[7]) - 1900;
    b = d + (int)((y - 1) * 365.25f);

    if((y % 4 == 0) && m > 1)
    {
        b += 1;
    }
    b -= 34995;

    return b;
}

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

    return Utils::FindPatternAddress(
        startAddr,
        endAddr,
        funcData->signature,
        funcData->mask
    );
}

static bool FindFuncBySignature(const moduleinfo_t &engineModule)
{
    uint8_t *moduleStartAddr = engineModule.baseAddr;
    uint8_t *moduleEndAddr = moduleStartAddr + engineModule.imageSize;
    const int signatureCount = 2;

    static const char *signatureArray[signatureCount] =
    {
        "\xA1\x00\x00\x00\x00\x83\xEC\x08\x00\x33\x00\x85\xC0",
        "\x55\x8B\xEC\x83\xEC\x08\xA1\x00\x00\x00\x00\x56\x33\xF6\x85\xC0\x0F\x85\x00\x00\x00\x00\x53\x33\xDB\x8B\x04\x9D"
    };
    static const char *maskArray[signatureCount] =
    {
        "x????xxx?x?xx",
        "xxxxxxx????xxxxxxx????xxxxxx"
    };

    for (int i = 0; i < signatureCount; ++i)
    {
        g_pfnGetBuildNumber = (int(*)())Utils::FindPatternAddress(
            moduleStartAddr,
            moduleEndAddr,
            signatureArray[i],
            maskArray[i]
        );

        if (g_pfnGetBuildNumber && g_pfnGetBuildNumber() > 0)
            return true;
    }

    return false;
}

static bool FindFuncByDateString(const moduleinfo_t &engineModule)
{
    const char *patternStr = "Jan";
    uint8_t *moduleStartAddr = engineModule.baseAddr;
    uint8_t *moduleEndAddr = moduleStartAddr + engineModule.imageSize;
    uint8_t *scanStartAddr = moduleStartAddr;

    while (true)
    {
        uint8_t *stringAddr = (uint8_t*)Utils::FindPatternAddress(
            scanStartAddr, moduleEndAddr,
            patternStr, "xxxx"
        );

        if (stringAddr)
        {
            const int scanOffset = 64; // offset for finding date string
            uint8_t *probeAddr = stringAddr - scanOffset;
            const char *dateString = FindDateString(probeAddr, scanOffset);
            if (dateString)
            {
                g_iBuildNumber = GetBuildNumberByDate(dateString);
                return true;
            }
            else
            {
                scanStartAddr = stringAddr + 1;
                continue;
            }
        }
        else
            return false;
    }
}

bool FindBuildNumberFunc(const moduleinfo_t &engineModule)
{
    if (FindFuncBySignature(engineModule))
        return true;
    else
        return FindFuncByDateString(engineModule);
}

int GetBuildNumber()
{
    if (g_pfnGetBuildNumber)
        return g_pfnGetBuildNumber();
    else if (g_iBuildNumber)
        return g_iBuildNumber;
    else
        return 0;
}
