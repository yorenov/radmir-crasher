//
// Created by n08i40k on 14.12.2023.
//

#ifndef ZENO_URPROJECT_REWRITE_D3DRAW_H
#define ZENO_URPROJECT_REWRITE_D3DRAW_H

#include <d3dx9.h>
#include <renderer.h>
#include <optional>

#include "utils/math/Vector3.h"

namespace D3Draw {
    void Draw3DBox(const Vector3f& startVector, const Vector3f& endVector,
                   std::optional<color_t> colorLine = D3DCOLOR_RGBA(255, 255, 255, 255),
                   std::optional<color_t> colorSide = D3DCOLOR_RGBA(150, 150, 150, 100));

    void Draw3DCircle(const Vector3f& startPos, float radius, uint8_t points,
                      color_t color = D3DCOLOR_XRGB(255, 255, 255));
}

#endif //ZENO_URPROJECT_REWRITE_D3DRAW_H
