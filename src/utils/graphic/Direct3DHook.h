#pragma once

#include <kthook/kthook.hpp>
#include <d3d9.h>

typedef HRESULT ( __stdcall*Direct3DPresentSign )(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND,
														CONST RGNDATA*);
typedef HRESULT ( __stdcall*Direct3DResetSign )(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT ( __stdcall*Direct3DAfterResetSign )(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*, HRESULT&);

class D3D9Hook
{
public:
	kthook::kthook_signal<Direct3DPresentSign> m_ktsPresent;
	kthook::kthook_signal<Direct3DResetSign>   m_ktsReset;

	D3D9Hook() = default;

	~D3D9Hook();

	void Setup(Direct3DPresentSign present, Direct3DResetSign reset, Direct3DAfterResetSign afterReset);
};
