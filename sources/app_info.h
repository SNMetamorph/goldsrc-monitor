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

#pragma once
#define MACRO_TO_STRING2(s)     #s
#define MACRO_TO_STRING(s)      MACRO_TO_STRING2(s)

#define DEFAULT_LIBRARY_NAME    "gsm-library.dll"
#define DEFAULT_PROCESS_NAME    "hl.exe";

#define APP_TITLE_STR           "GoldSrc Monitor"
#define APP_GITHUB_LINK         "https://github.com/SNMetamorph/goldsrc-monitor"
#define APP_BUILD_DATE          (__DATE__ " " __TIME__)
#define APP_VERSION_MAJOR       3
#define APP_VERSION_MINOR       0
#define APP_VERSION_STRING      MACRO_TO_STRING(APP_VERSION_MAJOR)      \
                                "." MACRO_TO_STRING(APP_VERSION_MINOR)  \
                                "\0"
