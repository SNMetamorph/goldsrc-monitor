#pragma once
#define MACRO_TO_STRING2(s)     #s
#define MACRO_TO_STRING(s)      MACRO_TO_STRING2(s)

#define APP_VERSION_MAJOR       2
#define APP_VERSION_MINOR       1
#define APP_VERSION_STRING      MACRO_TO_STRING(APP_VERSION_MAJOR)      \
                                "." MACRO_TO_STRING(APP_VERSION_MINOR)  \
                                "\0"
