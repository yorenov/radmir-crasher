#pragma once

#include <Windows.h>
#include <string>

namespace SignFinder
{
	void Parse(const char* combo, char* pattern, char* mask);

	char* ToChar(const wchar_t* string);

	char* ScanBasic(const char* pattern, const char* mask, char* begin, const intptr_t size);

	char* ScanInternal(const char* pattern, const char* mask, char* begin, const intptr_t size);

	char* ScanModIn(const char* pattern, const char* mask, const std::string& modName);

	char* ScanPatternCombo(const char* combo, const char* mod);
}
