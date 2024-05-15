#include "GameUtils.h"

#include <game_sa/CCamera.h>
#include <sampapi.h>

namespace GameUtils
{
	void ToggleCursor(const bool show, const bool lockControls)
	{
		if (show)
		{
			if (lockControls)
			{
				samp::RefGame()->SetCursorMode(samp::CURSOR_LOCKCAMANDCONTROL, false);
			} else {
				samp::RefGame()->SetCursorMode(samp::CURSOR_LOCKCAM, false);
			}
		}
		else
		{
			samp::RefGame()->SetCursorMode(samp::CURSOR_NONE, true);
		}
	}

    bool IsOnScreen(const Vector3f& position)
	{
		return TheCamera.IsSphereVisible(CVector(position.x, position.y, position.z), 0.01f);
	}

	float GetRandomF(const float min, const float max)
	{
		return min + rand() / (RAND_MAX / (max - min));
	}

	bool WorldToScreen(const Vector3f& inWorldPosition, Vector3f& outScreenPosition)
	{
		const auto   dWorldPosition = D3DXVECTOR3(inWorldPosition.x, inWorldPosition.y, inWorldPosition.z);
		D3DXVECTOR3  dScreenPosition;
		D3DVIEWPORT9 viewPort;
		D3DXMATRIX   view, projection, world;

		g_pDevice->GetViewport(&viewPort);
		g_pDevice->GetTransform(D3DTS_VIEW, &view);
		g_pDevice->GetTransform(D3DTS_PROJECTION, &projection);
		D3DXMatrixIdentity(&world);

		D3DXVec3Project(&dScreenPosition, &dWorldPosition, &viewPort, &projection, &view, &world);

        outScreenPosition = Vector3f(dScreenPosition);

		if (dScreenPosition.z < 1)
		{
			return true;
		}
		return false;
	}
};
