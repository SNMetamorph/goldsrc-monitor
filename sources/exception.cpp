#include <cstdio>
#include <cstring>
#include "exception.h"

CException::CException(const char *_desc, const char *_func_name, const char *_file_name, int _line)
{
	func_name = _func_name;
	file_path = _file_name;
	description = _desc;
	line = _line;
}

CException::~CException()
{

}

const char *CException::getFormattedMessage()
{
	snprintf(
		message_buffer, 
		sizeof(message_buffer), 
		"%s() [%s:%d]: %s\n", 
		func_name, 
		getFileName(), 
		line, 
		description
	);
	return message_buffer;
}

const char *CException::getDescription()
{
	return description;
}

const char *CException::getFunctionName()
{
	return func_name;
}

const char *CException::getFileName()
{
	return strrchr(file_path, '\\') + 1;
}

int CException::getLineNumber()
{
	return line;
}
