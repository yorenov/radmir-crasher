#pragma once

#include <game_sa/CVector.h>

#include "models/ModuleBase.h"
#include "utils/settings/Config.h"
#include "ktcoro_wait.hpp"
#include "sampapi/sampapi.h"

#include <optional>


class Crasher : ModuleBase
{
public:
	bool enabled;
    std::chrono::steady_clock::time_point lastCrash;

    std::vector<ktwait> tasks;
    ktcoro_tasklist taskManager;

	struct Config
	{
		float radius = 10.f;
        bool autocrasher = false;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, radius, autocrasher)
	};

	ConfigEntry<Config> configEntry{"crasher", {}};
	Config* config;

    std::optional<sampapi::ID> selectedVehicleId;

	Crasher();

    void crash();
	static void gameProcess(Crasher* self);
    static void draw(Crasher* self, IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
};
