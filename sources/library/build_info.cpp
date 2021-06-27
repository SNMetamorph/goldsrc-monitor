#include "build_info.h"
#include "exception.h"
#include "utils.h"
#include <string>
#include <fstream>
#include <filesystem>

void CBuildInfo::Initialize(const moduleinfo_t &engineModule)
{
    std::vector<uint8_t> fileContents;
    if (LoadBuildInfoFile(fileContents)) {
        ParseBuildInfo(fileContents);
    }
    else {
        EXCEPT("failed to load build info file");
    }
    if (!FindBuildNumberFunc(engineModule))
    {
        if (!ApproxBuildNumber(engineModule)) {
            EXCEPT("failed to approximate engine build number");
        }
    }
    m_iActualEntryIndex = FindActualInfoEntry();
    if (m_iActualEntryIndex < 0) {
        EXCEPT("no one build info entries parsed");
    }
}

void *CBuildInfo::FindFunctionAddress(FunctionType funcType, void *startAddr, void *endAddr) const
{
    CMemoryPattern funcPattern = m_InfoEntries[m_iActualEntryIndex].GetFunctionPattern(funcType);
    if (!endAddr)
        endAddr = (uint8_t *)startAddr + funcPattern.GetLength();

    return Utils::FindPatternAddress(
        startAddr,
        endAddr,
        funcPattern
    );
}

int CBuildInfo::GetBuildNumber() const
{
    if (m_pfnGetBuildNumber)
        return m_pfnGetBuildNumber();
    else if (m_iBuildNumber)
        return m_iBuildNumber;
    else
        return 0;
}

int CBuildInfo::DateToBuildNumber(const char *date) const
{
    int m = 0, d = 0, y = 0;
    static int b = 0;
    static int monthDaysCount[12] = { 
        31, 28, 31, 30, 
        31, 30, 31, 31, 
        30, 31, 30, 31 
    };
    static const char *monthNames[12] = { 
        "Jan", "Feb", "Mar", 
        "Apr", "May", "Jun", 
        "Jul", "Aug", "Sep", 
        "Oct", "Nov", "Dec" 
    };

    if (b != 0)
        return b;

    for (m = 0; m < 11; m++)
    {
        if (!strnicmp(&date[0], monthNames[m], 3))
            break;
        d += monthDaysCount[m];
    }

    d += atoi(&date[4]) - 1;
    y = atoi(&date[7]) - 1900;
    b = d + (int)((y - 1) * 365.25f);

    if ((y % 4 == 0) && m > 1)
    {
        b += 1;
    }
    b -= 34995;

    return b;
}

const char *CBuildInfo::FindDateString(uint8_t *startAddr, int maxLen) const
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

bool CBuildInfo::LoadBuildInfoFile(std::vector<uint8_t> &fileContents)
{
    std::wstring libraryDirPath;
    Utils::GetLibraryDirectory(libraryDirPath);
    std::ifstream file(std::filesystem::path(libraryDirPath + L"\\build_info.json"), std::ios::in | std::ios::binary);
    fileContents.clear();
    if (file.is_open())
    {
#ifdef max
#undef max
        file.ignore(std::numeric_limits<std::streamsize>::max());
#endif
        size_t length = file.gcount();
        file.clear();
        file.seekg(0, std::ios_base::beg);
        fileContents.reserve(length);
        fileContents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        fileContents.push_back('\0');
        file.close();
        return true;
    }
    else {
        return false;
    }
}

void CBuildInfo::ParseBuildInfo(std::vector<uint8_t> &fileContents)
{
    rapidjson::Document doc;
    doc.Parse(reinterpret_cast<const char *>(fileContents.data()));

    if (!doc.IsObject()) {
        EXCEPT("JSON: build info document root is not object");
    }
    if (!doc.HasMember("build_number_signatures")) {
        EXCEPT("JSON: build info document hasn't member build_number_signatures");
    }
    if (!doc.HasMember("engine_builds_info")) {
        EXCEPT("JSON: build info document hasn't member engine_builds_info");
    }

    const rapidjson::Value &buildSignatures = doc["build_number_signatures"];
    for (size_t i = 0; i < buildSignatures.Size(); ++i) {
        m_BuildNumberSignatures.emplace_back(buildSignatures[i].GetString());
    }

    const rapidjson::Value &engineBuilds = doc["engine_builds_info"];
    for (size_t i = 0; i < engineBuilds.Size(); ++i)
    {
        CBuildInfoEntry infoEntry;
        ParseBuildInfoEntry(infoEntry, engineBuilds[i]);
        m_InfoEntries.push_back(infoEntry);
    }

    if (doc.HasMember("game_specific_builds_info"))
    {
        const rapidjson::Value &gameSpecificBuilds = doc["game_specific_builds_info"];
        for (size_t i = 0; i < gameSpecificBuilds.Size(); ++i)
        {
            CBuildInfoEntry infoEntry;
            const rapidjson::Value &entryObject = gameSpecificBuilds[i];
            if (entryObject.HasMember("process_name")) 
            {
                ParseBuildInfoEntry(infoEntry, entryObject);
                infoEntry.SetGameProcessName(entryObject["process_name"].GetString());
                m_InfoEntries.push_back(infoEntry);
            }
            else {
                EXCEPT("JSON: parsed game specific build info without process name");
            }
        }
    }
}

void CBuildInfo::ParseBuildInfoEntry(CBuildInfoEntry &destEntry, const rapidjson::Value &jsonObject)
{
    if (jsonObject.HasMember("number")) {
        destEntry.SetBuildNumber(jsonObject["number"].GetInt());
    }
    if (jsonObject.HasMember("cl_engfuncs_offset")) {
        destEntry.SetClientEngfuncsOffset(jsonObject["cl_engfuncs_offset"].GetUint64());
    }
    if (jsonObject.HasMember("sv_engfuncs_offset")) {
        destEntry.SetServerEngfuncsOffset(jsonObject["sv_engfuncs_offset"].GetUint64());
    }
    if (jsonObject.HasMember("signatures"))
    {
        const rapidjson::Value &signatures = jsonObject["signatures"];
        destEntry.SetFunctionPattern(FUNCTYPE_SPR_LOAD, CMemoryPattern(signatures["SPR_Load"].GetString()));
        destEntry.SetFunctionPattern(FUNCTYPE_SPR_FRAMES, CMemoryPattern(signatures["SPR_Frames"].GetString()));
        if (signatures.HasMember("PrecacheModel")) {
            destEntry.SetFunctionPattern(FUNCTYPE_PRECACHE_MODEL, CMemoryPattern(signatures["PrecacheModel"].GetString()));
        }
        if (signatures.HasMember("PrecacheSound")) {
            destEntry.SetFunctionPattern(FUNCTYPE_PRECACHE_SOUND, CMemoryPattern(signatures["PrecacheSound"].GetString()));
        }
    }

    if (!destEntry.Validate()) {
        EXCEPT("JSON: parsed empty build info entry, check if signatures/offsets are set");
    }
}

bool CBuildInfo::ApproxBuildNumber(const moduleinfo_t &engineModule)
{
    CMemoryPattern datePattern("Jan", 4);
    uint8_t *moduleStartAddr = engineModule.baseAddr;
    uint8_t *moduleEndAddr = moduleStartAddr + engineModule.imageSize;
    uint8_t *scanStartAddr = moduleStartAddr;
    while (true)
    {
        uint8_t *stringAddr = (uint8_t *)Utils::FindPatternAddress(
            scanStartAddr, moduleEndAddr, datePattern
        );

        if (stringAddr)
        {
            const int scanOffset = 64; // offset for finding date string
            uint8_t *probeAddr = stringAddr - scanOffset;
            const char *dateString = FindDateString(probeAddr, scanOffset);
            if (dateString)
            {
                m_iBuildNumber = DateToBuildNumber(dateString);
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

bool CBuildInfo::FindBuildNumberFunc(const moduleinfo_t &engineModule)
{
    uint8_t *moduleStartAddr = engineModule.baseAddr;
    uint8_t *moduleEndAddr = moduleStartAddr + engineModule.imageSize;
    for (size_t i = 0; i < m_BuildNumberSignatures.size(); ++i)
    {
        m_pfnGetBuildNumber = (int(*)())Utils::FindPatternAddress(
            moduleStartAddr,
            moduleEndAddr,
            m_BuildNumberSignatures[i]
        );

        if (m_pfnGetBuildNumber && m_pfnGetBuildNumber() > 0)
            return true;
    }
    return false;
}

int CBuildInfo::FindActualInfoEntry()
{
    if (m_InfoEntries.size() < 1) {
        return -1;
    }
    else
    {
        std::string processName;
        int currBuildNumber = GetBuildNumber();
        const int lastEntryIndex = m_InfoEntries.size() - 1;
        int actualEntryIndex = lastEntryIndex;
        
        // first check among game-specific builds
        Utils::GetGameProcessName(processName);
        for (size_t i = 0; i < m_InfoEntries.size(); ++i)
        {
            const CBuildInfoEntry &buildInfo = m_InfoEntries[i];
            const std::string &targetName = buildInfo.GetGameProcessName();
            if (targetName.length() > 0 && targetName.compare(processName) == 0) {
                return i;
            }
        }

        // and then among engine builds
        std::sort(m_InfoEntries.begin(), m_InfoEntries.end());
        for (int i = 0; i < lastEntryIndex; ++i)
        {
            const CBuildInfoEntry &buildInfo = m_InfoEntries[i];
            const CBuildInfoEntry &nextBuildInfo = m_InfoEntries[i + 1];
            const int nextBuildNumber = nextBuildInfo.GetBuildNumber();
            if (nextBuildNumber > currBuildNumber) // valid only if build info entries sorted ascending
            {
                actualEntryIndex = i;
                break;
            }
        }
        return actualEntryIndex;
    }
}
