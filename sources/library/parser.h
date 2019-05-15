#pragma once
#include "globals.h"
#include <stdint.h>

typedef void(*sect_handler_t)(char *, char *);

class CConfigParser
{
public:
	CConfigParser();
	bool openConfig(const char *path);
	void parseSection(char *&file_iter, char *sect_name);
	void setLabelHandler(sect_handler_t addr);

private:
	char *loadFile(const char *path);
	void parseFile(char *&file_iter, char *token);

	sect_handler_t label_handler;
	char *file_desc;
};