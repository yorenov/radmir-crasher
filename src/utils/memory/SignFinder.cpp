#include "SignFinder.h"

#include <psapi.h>
#include <winternl.h>

#include <iostream>
#include <cassert>

PPEB GetPPEB()
{
	return reinterpret_cast<PPEB>(__readfsdword(0x30));  // NOLINT(performance-no-int-to-ptr)
}

LDR_DATA_TABLE_ENTRY* GetLDREntry(const std::string& name)
{
	LDR_DATA_TABLE_ENTRY* ldr = nullptr;

	const auto peb = GetPPEB();

	const auto head = peb->Ldr->InMemoryOrderModuleList;

	auto curr = head;

	while (curr.Flink != head.Blink)
	{
		if (const auto mod = CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks); mod->
			FullDllName.Buffer)
		{
			const char* cName = SignFinder::ToChar(mod->FullDllName.Buffer);

			if (_stricmp(cName, name.c_str()) == 0)
			{
				ldr = mod;
				break;
			}
			delete[] cName;
		}
		curr = *curr.Flink;
	}
	return ldr;
}

void SignFinder::Parse(const char* combo, char* pattern, char* mask)
{
	char         lastChar = ' ';
	unsigned int j        = 0;

	for (unsigned int i = 0; i < strlen(combo); i++)
	{
		if ((combo[i] == '?' || combo[i] == '*') && (lastChar != '?' && lastChar != '*'))
		{
			pattern[j] = mask[j] = '?';
			j++;
		}

		else if (isspace(lastChar))
		{
			pattern[j] = lastChar = static_cast<char>(strtol(&combo[i], 0, 16));
			mask[j]    = 'x';
			j++;
		}
		lastChar = combo[i];
	}
	pattern[j] = mask[j] = '\0';
}

char* SignFinder::ToChar(const wchar_t* string)
{
	const auto len = wcslen(string) + 1;
	auto       dst = new char[len];
	size_t     numCharsRead;
	wcstombs_s(&numCharsRead, dst, len, string, _TRUNCATE);
	return dst;
}

char* SignFinder::ScanBasic(const char* pattern, const char* mask, char* begin, const intptr_t size)
{
	const intptr_t patternLen = strlen(mask);

	for (int i = 0; i < size; i++)
	{
		bool found = true;
		for (int j = 0; j < patternLen; j++)
		{
			if (mask[j] != '?' && pattern[j] != *reinterpret_cast<char*>(reinterpret_cast<intptr_t>(begin) + i + j))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return (begin + i);
		}
	}
	return nullptr;
}

char* SignFinder::ScanInternal(const char* pattern, const char* mask, char* begin, const intptr_t size)
{
	char*                    match{nullptr};
	MEMORY_BASIC_INFORMATION mbi{};

	for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
	{
		if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS)
			continue;

		match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

		if (match != nullptr)
		{
			break;
		}
	}
	return match;
}

char* SignFinder::ScanModIn(const char* pattern, const char* mask, const std::string& modName)
{
	HANDLE     hProcess = GetCurrentProcess();
	MODULEINFO hModInfo;
	DWORD      cbNeeded;

	hProcess = GetCurrentProcess();
	assert(NULL != hProcess);

	if (HMODULE hMods[1024]; EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < static_cast<int>(cbNeeded / sizeof(HMODULE)); ++i)
		{
			if (char szModName[MAX_PATH]; GetModuleFileNameA(hMods[i], szModName, MAX_PATH))
			{
				if (std::string(szModName).contains(modName))
				{
					GetModuleInformation(hProcess, hMods[i], &hModInfo, sizeof(MODULEINFO));
					break;
				}
			}
		}
	}
	char* match = ScanInternal(pattern, mask, static_cast<char*>(hModInfo.lpBaseOfDll), hModInfo.SizeOfImage);

	return match;
}

char* SignFinder::ScanPatternCombo(const char* combo, const char* mod)
{
	char pattern[100];
	char mask[100];
	Parse(combo, pattern, mask);
	return ScanModIn(pattern, mask, mod);
}