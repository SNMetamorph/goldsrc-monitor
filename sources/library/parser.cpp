#include "parser.h"
#include <cstring>

CConfigParser::CConfigParser()
{
	file_desc		= nullptr;
	label_handler	= nullptr;
}

bool CConfigParser::openConfig(const char *file_path)
{
	char *file_iter;
	static char token[512];
	static char sect_name[32];

	file_desc = loadFile(file_path);
	file_iter = file_desc;
	if (!file_desc)
		return false;

	while (true)
	{
		parseFile(file_iter, token);
		if (!file_iter)
			break;	// reached end of file
		
		if (strcmp(token, "{") && strcmp(token, "}"))
		{
			if (strlen(token) > 1)
			{
				// store section name
				strncpy(sect_name, token, sizeof(sect_name));
				parseFile(file_iter, token);
				if (strcmp(token, "{") == 0)
					parseSection(file_iter, sect_name);
			}
		}
	}

	g_pClientEngFuncs->COM_FreeFile(file_desc);
	return true;
}

void CConfigParser::parseSection(char *&file_iter, char* sect_name)
{
	static char	key[32];
	static char	value[32];
	sect_handler_t handler = nullptr;

	// set proper handler function address
	if (strcmp(sect_name, "label"))
		handler = label_handler;
	else
		return;

	// parse first key
	parseFile(file_iter, key);
	while (strcmp(key, "}"))
	{
		parseFile(file_iter, value);
		handler(key, value);
		parseFile(file_iter, key);
	}
}

void CConfigParser::setLabelHandler(sect_handler_t addr)
{
	label_handler = addr;
}

char *CConfigParser::loadFile(const char *path)
{
	return (char*)g_pClientEngFuncs->COM_LoadFile(path, 0, nullptr);
}

void CConfigParser::parseFile(char *&file_iter, char *token)
{
	file_iter = g_pClientEngFuncs->COM_ParseFile(file_iter, token);
}