#pragma once
#define MACRO_TO_STRING2(s)     #s
#define MACRO_TO_STRING(s)      MACRO_TO_STRING2(s)

#define DEFAULT_LIBRARY_NAME    L"gsm-library.dll"
#define DEFAULT_PROCESS_NAME    L"hl.exe";

#define APP_TITLE_STR           "GoldSrc Monitor"
#define APP_GITHUB_LINK         "https://github.com/SNMetamorph/goldsrc-monitor"
#define APP_BUILD_DATE          (__DATE__ " " __TIME__)
#define APP_VERSION_MAJOR       2
#define APP_VERSION_MINOR       8
#define APP_VERSION_STRING      MACRO_TO_STRING(APP_VERSION_MAJOR)      \
                                "." MACRO_TO_STRING(APP_VERSION_MINOR)  \
                                "\0"
