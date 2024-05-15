#include "MemoryPatch.h"

#include <Windows.h>
#include <cassert>
#include "utils/other/ext_main.h"
#include "defines.h"

void PrintMemory(void* addr, const size_t len)
{
	assert(len);

	for (int i = 0; i < len; ++i)
	{
		debugWL("%02X", static_cast<char*>(addr)[i]);
	}

    debugWL("\n");
}


void MemPatch::operator()(void*      adress, const size_t size, const char* data, const bool autoPatch,
				const bool restoreInDestructor)
{
	assert(adress != nullptr);
	assert(size > 0);

	m_pDestAdress = adress;

	m_szPatchSize       = size;
	m_bPatched          = false;
	m_bResetAfterUnload = restoreInDestructor;

	// create buffers
	m_pNewData = std::malloc_s(m_szPatchSize);
	m_pOldData = std::malloc_s(m_szPatchSize);

	DWORD oldProt;
	VirtualProtect(m_pDestAdress, m_szPatchSize, PAGE_READWRITE, &oldProt);
	{
		memcpy_s(m_pOldData, m_szPatchSize,
				m_pDestAdress, m_szPatchSize);

		if (data != nullptr)
		{
			// set new buffer
			memcpy_s(m_pNewData, m_szPatchSize,
					data, m_szPatchSize);
		}
		else
		{
			// set new buffer
			memset(m_pNewData, 0x90, m_szPatchSize);
		}
	}

	VirtualProtect(m_pDestAdress, m_szPatchSize, oldProt, &oldProt);

	if (autoPatch)
	{
		Patch();
	}
}

MemPatch::MemPatch()
{
	m_pDestAdress = nullptr;
	m_pNewData    = nullptr;
	m_pOldData    = nullptr;

	m_bPatched          = false;
	m_bResetAfterUnload = false;

	m_szPatchSize = 0;
}

MemPatch::~MemPatch()
{
	if (m_bResetAfterUnload && m_bPatched)
	{
		Patch(true);
	}

	free(m_pOldData);
	free(m_pNewData);
}

void* MemPatch::GetAddress() const
{
	return m_pDestAdress;
}


void MemPatch::Patch(const bool restore)
{
	DWORD oldProt;
	VirtualProtect(m_pDestAdress, m_szPatchSize, PAGE_READWRITE, &oldProt);

	if (restore)
	{
		if (m_bPatched)
		{
			memcpy_s(m_pDestAdress, m_szPatchSize,
					m_pOldData, m_szPatchSize);
			m_bPatched = false;
		}
	}
	else
	{
		if (!m_bPatched)
		{
			memcpy_s(m_pDestAdress, m_szPatchSize,
					m_pNewData, m_szPatchSize);

			m_bPatched = true;
		}
	}

	VirtualProtect(m_pDestAdress, m_szPatchSize, oldProt, &oldProt);
}

void MemPatch::SetOriginalMem(const size_t size, const char* data) const
{
	assert(size != m_szPatchSize);

	memcpy_s(m_pOldData, m_szPatchSize,
			data, m_szPatchSize);
}