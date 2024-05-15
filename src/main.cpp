#define ZENO_VER "2.0 in-dev"
#define ZENO "ZENO urProject v" ZENO_VER

#include <RakHook/rakhook.hpp>

#include "main/PluginState.h"
#include "main/ASIController.h"

#include "plugin.h"
#include "sampapi.h"

#include "main/LoadController.h"
#include <kthook/kthook.hpp>

#include "defines.h"

#include "utils/controls/KeyCheck.h"
#include "models/ModuleBase.h"
#include "main/ModuleLoader.h"
#include "utils/settings/Config.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LoadController g_cLoadController;

using namespace std::chrono_literals;
using namespace plugin;

LoadController g_cLoadController;

bool g_bSampInitialized = false;

HRESULT __stdcall Direct3DPresent(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect,
                                  HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	if (!ASI::components.direct3D9Data)
	{
		g_pDevice = pDevice;
		g_pRender = new cRender(pDevice, true);
		ASI::components.direct3D9Data = true;
	}

	if (*reinterpret_cast<bool*>(0xBA67A4)) return 0;

	g_pRender->BeginDraw();

	for (auto* pModule : g_modules)
	{
		if (const auto& present = pModule->getProperties().direct3DPresent;
			present.has_value())
		{
			present.value()(pModule, pDevice, pSourceRect, pDestRect, hDestWindowOverride,
			                                 pDirtyRegion);
		}
	}

	g_pRender->EndDraw();

	return 0;
}

HRESULT __stdcall Direct3DReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (ASI::components.direct3D9Data)
	{
		g_pRender->OnLostDevice();

		for (auto* pModule : g_modules)
		{
			if (const auto& reset = pModule->getProperties().direct3DReset;
				reset.has_value())
			{
				reset.value()(pModule, pDevice, pPresentationParameters);
			}
		}
	}
	return 0;
}

HRESULT __stdcall Direct3DAfterReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters,
                                     HRESULT& returnValue)
{
	if (ASI::components.direct3D9Data && returnValue >= 0)
		g_pRender->OnResetDevice();
	return 0;
}

void FillInitStages()
{
    g_cLoadController.add([]() -> bool {
        ASI::hooks.direct3D9.Setup(Direct3DPresent, Direct3DReset, Direct3DAfterReset);

        ASI::components.direct3D9Hook = true;
        return true;
    }, "Install Direct3D9 hook");
    g_cLoadController.add([]() -> bool {
        if (!std::filesystem::exists(GetProjectDirectory()))
            std::filesystem::create_directories(GetProjectDirectory());

        if (const auto pConfig = ConfigManager::GetInstance(); !pConfig->Load()) {
            pConfig->Save();

            bool result = pConfig->Load();
            assert(result);
        }
        return true;
    }, "Init ConfigManager");
    g_cLoadController.add([]() -> bool {
        bool rakhookInitialized = rakhook::initialize();
        assert(rakhookInitialized);

        return true;
    }, "Init RakHook");
    g_cLoadController.add([]() -> bool {
        if (samp::RefNetGame()->GetState() != 5)
            return false;
        LoadModules();

        return true;
    }, "Load modules.");
    g_cLoadController.add([]() -> bool {
        ASI::components.finished = true;
        samp::RefChat()->AddMessage(D3DCOLOR_XRGB(0, 255, 0), "Hi!");
        return true;
    }, "Init completed message");
}

void MainLoop()
{
	if (!g_bSampInitialized) return;

	if (!g_cLoadController.process())
		return;

	KeyCheck::Update();

    if (!*reinterpret_cast<bool*>(0xBA67A4) && !samp::RefInputBox()->m_bEnabled && !samp::RefDialog()->m_bIsActive)
    {
        if (KeyCheck::Check(KB_LCONTROL) && KeyCheck::Check(KB_LSHIFT) && KeyCheck::CheckJustDown(KB_LBUTTON))
        {
            //ASI::Unload();
        }
    }


    using namespace std::chrono;
#define HRC_NOW high_resolution_clock::now
    static auto lastConfigSave = HRC_NOW();

    if (duration<float, std::milli>(HRC_NOW() - lastConfigSave).count() / 1000 >= 10)
    {
        debugWL("Planned config save...");
        lastConfigSave = HRC_NOW();
        ConfigManager::GetInstance()->Save();
    }
#undef HRC_NOW

    for (auto* pModule : g_modules)
    {
        if (const ModuleProperties& stProps = pModule->getProperties();
            stProps.mainLoop.has_value())
        {
            stProps.mainLoop.value()(pModule);
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
	case (DLL_PROCESS_ATTACH):
	{
		setlocale(LC_ALL, ".UTF8");  // NOLINT(cert-err33-c)
#ifdef _DEBUG
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
#endif

		DisableThreadLibraryCalls(hModule);

		ASI::hooks.gameProcess.set_dest(0x53BEE0);
		ASI::hooks.gameProcess.after.connect([](const auto& pHook)
		{
			MainLoop();
			return std::nullopt;
		});
		ASI::hooks.gameProcess.install();


        static kthook::kthook_signal<void(__cdecl*)()> g_ktsSampInitHook;
		g_ktsSampInitHook.set_dest(0x58A330);
		g_ktsSampInitHook.after.connect([&](const auto& pHook)
		{
			g_bSampInitialized = true;

			g_ktsSampInitHook.remove(); // hook function once

			return std::nullopt;
		});
		g_ktsSampInitHook.install();

		FillInitStages();

		break;
	}
	case DLL_PROCESS_DETACH:
	{
#ifdef _DEBUG
		FreeConsole();
#endif
		break;
	}
	default:
		break;
	}
	return TRUE;
}
