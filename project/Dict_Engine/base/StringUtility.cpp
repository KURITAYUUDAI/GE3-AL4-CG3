#include "StringUtility.h"
#include <Windows.h>

namespace StringUtility
{
	std::wstring ConvertString(const std::string& str)
	{
		if (str.empty()) {
			return std::wstring();
		}

		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

	std::string ConvertString(const std::wstring& str)
	{
		if (str.empty()) {
			return std::string();
		}

		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;
	}

	std::u32string ConvertUtf8ToUtf32(const std::string& str)
	{
		const std::wstring wide = ConvertString(str);
		std::u32string result;
		result.reserve(wide.size());

		for (size_t i = 0; i < wide.size(); ++i)
		{
			const wchar_t high = wide[i];
			if (high >= 0xD800 && high <= 0xDBFF &&
				i + 1 < wide.size())
			{
				const wchar_t low = wide[i + 1];
				if (low >= 0xDC00 && low <= 0xDFFF)
				{
					const char32_t codepoint =
						0x10000 +
						((static_cast<char32_t>(high) - 0xD800) << 10) +
						(static_cast<char32_t>(low) - 0xDC00);
					result.push_back(codepoint);
					++i;
					continue;
				}
			}

			result.push_back(static_cast<char32_t>(high));
		}

		return result;
	}

	std::string ConvertUtf32ToUtf8(const std::u32string& str)
	{
		std::wstring wide;
		wide.reserve(str.size());

		for (char32_t c : str)
		{
			if (c <= 0xFFFF)
			{
				wide.push_back(static_cast<wchar_t>(c));
			}
			else
			{
				const char32_t value = c - 0x10000;
				wide.push_back(static_cast<wchar_t>(0xD800 + (value >> 10)));
				wide.push_back(static_cast<wchar_t>(0xDC00 + (value & 0x3FF)));
			}
		}

		return ConvertString(wide);
	}
}
