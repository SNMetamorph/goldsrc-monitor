#pragma once
#include "moduleinfo.h"
#include "string_stack.h"

void    ProgramInit();
void	AssignDisplayMode();
int		GetStringWidth(const char *str);
void    DrawStringStack(int marginRight, int marginUp, const CStringStack &stringStack);
bool    IsSoftwareRenderer();
