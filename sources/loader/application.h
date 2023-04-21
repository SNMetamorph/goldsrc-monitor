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
