#pragma once
#include "funcdata.h"
#include "moduleinfo.h"

struct buildinfo_entry_t
{
    int number;
    funcdata_t functionData[FUNCTYPE_COUNT];
};

int GetBuildNumber();
void *FindFunctionAddress(functype_t funcType, void *startAddr, void *endAddr = nullptr);
bool FindBuildNumberFunc(const moduleinfo_t &engineModule);
