#pragma once

#include <optional>

#include <plugin.h>
#include <game_sa/CVector.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <main/PluginState.h>

#include "utils/math/Vector3.h"

namespace GameUtils
{
	template<class firstVector, class secondVector>
    inline float Distance3D(const firstVector& a, const secondVector& b)
	{
		return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
	}

	template<class firstVector, class secondVector>
	inline float Distance2D(const firstVector& a, const secondVector& b)
	{
		return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	void ToggleCursor(bool show, bool lockControls = false);

	bool IsOnScreen(const Vector3f& position);

	float GetRandomF(float min, float max);

	bool WorldToScreen(const Vector3f& inWorldPosition, Vector3f& outScreenPosition);
}
