#pragma once
#include <string>

namespace StringUtility
{
	std::wstring ConvertString(const std::string& str);

	std::string ConvertString(const std::wstring& str);

	std::u32string ConvertUtf8ToUtf32(const std::string& str);

	std::string ConvertUtf32ToUtf8(const std::u32string& str);
}
