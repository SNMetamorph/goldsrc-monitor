#pragma once

// Universal macros
#define EXCEPT(m) (throw CException(m, __FUNCTION__, __FILE__, __LINE__))

class CException
{
public:
	CException(const char *_desc, const char *_func_name, const char *_file_path, int _line);
	~CException();
	const char *getFormattedMessage();
	const char *getDescription();
	const char *getFunctionName();
	const char *getFileName();
	int			getLineNumber();
	char message_buffer[256];

private:
	int line;
	const char *description;
	const char *func_name;
	const char *file_path;
};

