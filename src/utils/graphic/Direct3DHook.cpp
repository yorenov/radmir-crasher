#include "Direct3DHook.h"

#include <kthook/kthook.hpp>
#include <d3d9.h>

typedef HRESULT ( __stdcall*Direct3DPresentSign )(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND,
														CONST RGNDATA*);
typedef HRESULT ( __stdcall*Direct3DResetSign )(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT ( __stdcall*Direct3DAfterResetSign )(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*, HRESULT&);

DWORD FindDevice(const DWORD len)
{
	char infoBuf[MAX_PATH];
	GetSystemDirectoryA(infoBuf, MAX_PATH);
	strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");

	auto dwObjBase = reinterpret_cast<DWORD>(LoadLibraryA(infoBuf));
	while (dwObjBase < dwObjBase + len)
	{
        dwObjBase++;

		if (*reinterpret_cast<WORD*>(dwObjBase + 0x00) == 0x06C7 &&
			*reinterpret_cast<WORD*>(dwObjBase + 0x06) == 0x8689 &&
			*reinterpret_cast<WORD*>(dwObjBase + 0x0C) == 0x8689)
		{
			dwObjBase += 2;
			break;
		}
	}
	return dwObjBase;
}

void* GetDeviceAddress(const int vTableIndex)
{
	PDWORD vTable;
	*reinterpret_cast<PDWORD>(&vTable) = *reinterpret_cast<PDWORD>(FindDevice(0x128000));

	return reinterpret_cast<void*>(vTable[vTableIndex]);
}


D3D9Hook::~D3D9Hook()
{
	this->m_ktsPresent.remove();
	this->m_ktsReset.remove();
}

void D3D9Hook::Setup(Direct3DPresentSign present, Direct3DResetSign reset, Direct3DAfterResetSign afterReset)
{
	assert(present != nullptr);
	assert(reset != nullptr);
	assert(afterReset != nullptr);

	const auto pPresent = GetDeviceAddress(17);
	const auto pReset = GetDeviceAddress(16);

	assert(pPresent != nullptr);
	assert(pReset != nullptr);

	this->m_ktsPresent.set_dest(pPresent);
	this->m_ktsReset.set_dest(pReset);

	this->m_ktsPresent.before.connect([present](const auto&    pHook, IDirect3DDevice9* pDevice,
	                                            CONST RECT*    pSourceRect,
	                                            CONST RECT*    pDestRect, HWND hDestWindowOverride,
	                                            CONST RGNDATA* pDirtyRegion)
	{
		present(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		return std::nullopt;
	});
	this->m_ktsReset.before.connect([reset](const auto&            pHook, IDirect3DDevice9* pDevice,
	                                        D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		reset(pDevice, pPresentationParameters);
		return std::nullopt;
	});
	this->m_ktsReset.after.connect([afterReset](const auto& pHook, HRESULT& returnValue, IDirect3DDevice9* pDevice,
	                                            D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		afterReset(pDevice, pPresentationParameters, returnValue);
		return true;
	});

	this->m_ktsPresent.install();
	this->m_ktsReset.install();
}