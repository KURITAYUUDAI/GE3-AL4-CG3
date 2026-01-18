#include "Logger.h"
#include <Windows.h>
#include <string>

namespace Logger
{
	void Log(const std::string& message)
	{
		OutputDebugStringA(message.c_str());
	}
}