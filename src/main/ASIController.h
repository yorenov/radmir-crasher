#pragma once

#include <RakHook/rakhook.hpp>
#include <kthook/kthook.hpp>

#include "utils/graphic/Direct3DHook.h"
#include "utils/settings/Config.h"

namespace ASI
{
	namespace Structs
	{
		struct ASIComponents
		{
			bool direct3D9Hook;     // CD3DHook
			bool direct3D9Data;     // Device, renderer, etc.
			bool finished;          // All hooks, modules, etc. has been initialized
		};

		struct ASIHooks
		{
			kthook::kthook_signal<void ( *)()>  gameProcess;
            D3D9Hook                            direct3D9;
		};
	}

	extern Structs::ASIComponents    components;
	extern Structs::ASIHooks         hooks;
};