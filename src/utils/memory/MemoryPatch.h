#pragma once

void PrintMemory(void* addr, const size_t len);

class MemPatch
{
	void* m_pDestAdress;

	void* m_pOldData;
	void* m_pNewData;

	size_t m_szPatchSize;

	bool m_bPatched;
	bool m_bResetAfterUnload;

public:
	void operator()(void* adress, const size_t size, const char* data, const bool autoPatch,
					bool restoreInDestructor);

	MemPatch();

	~MemPatch();

	[[nodiscard]] void* GetAddress() const;

	void Patch(bool restore = false);

	void SetOriginalMem(size_t size, const char* data) const;
};
