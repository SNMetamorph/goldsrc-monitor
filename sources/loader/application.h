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
#include "app_info.h"
#include "inject_strategy.h"
#include <stdint.h>
#include <string>
#include <memory>

class CApplication
{
public:
    static CApplication &GetInstance();
    int Run(int argc, char *argv[]);

private:
    CApplication() {};
    ~CApplication() {};

    void ParseParameters(int argc, char *argv[]);
    void StartMainLoop();
    void ReportError(const std::string &msg);
    void PrintTitleText();
    void InitInjectStrategy();

    size_t m_iInjectDelay = 3000;
    std::unique_ptr<IInjectStrategy> m_pInjectStrategy;
    std::string m_szProcessName = DEFAULT_PROCESS_NAME;
    std::string m_szLibraryName = DEFAULT_LIBRARY_NAME;
};
