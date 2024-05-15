//
// Created by n08i40k on 14.12.2023.
//

#include "D3Draw.h"
#include "defines.h"
#include "utils/game/GameUtils.h"
#include <cmath>

template<uint8_t N1, uint8_t N2>
void CalculateScreenPosition(Vector3<int16_t> (&drawablePoints)[N1][N2],
                             uint8_t x, uint8_t y,
                             const Vector3f& xVector,
                             const Vector3f& yVector,
                             const Vector3f& zVector) {
    Vector3f worldPosition;
    GameUtils::WorldToScreen({xVector.x, yVector.y, zVector.z}, worldPosition);

    drawablePoints[x][y] = worldPosition.convert<int16_t>();
}

template<uint8_t N1, uint8_t N2>
void DrawLine(const Vector3<int16_t> (&drawablePoints)[N1][N2],
              uint8_t fromX, uint8_t fromY,
              uint8_t toX, uint8_t toY,
              color_t color) {
    g_pRender->DrawLine(drawablePoints[fromX][fromY].x, drawablePoints[fromX][fromY].y,
                        drawablePoints[toX][toY].x, drawablePoints[toX][toY].y,
                        color);
}

template<uint8_t N1, uint8_t N2>
void DrawSide(const Vector3<int16_t> (&drawablePoints)[N1][N2],
              uint8_t point1X, uint8_t point1Y,
              uint8_t point2X, uint8_t point2Y,
              uint8_t point3X, uint8_t point3Y,
              uint8_t point4X, uint8_t point4Y,
              color_t color) {
    g_pRender->DrawQuad(
        drawablePoints[point1X][point1Y].x, drawablePoints[point1X][point1Y].y,
        drawablePoints[point2X][point2Y].x, drawablePoints[point2X][point2Y].y,
        drawablePoints[point3X][point3Y].x, drawablePoints[point3X][point3Y].y,
        drawablePoints[point4X][point4Y].x, drawablePoints[point4X][point4Y].y,
        RenderDrawType_Filled, color, 0, 0, 0);
}

void D3Draw::Draw3DBox(const Vector3f& startVector, const Vector3f& endVector,
                       const std::optional<color_t> colorLine,
                       const std::optional<color_t> colorSide)
{
    if (!colorSide.has_value() && !colorSide.has_value())
        return;

    Vector3<int16_t> drawablePoints[2][4];

    // Calculate box points
    CalculateScreenPosition(drawablePoints, 0, 0, startVector, startVector, startVector);
    CalculateScreenPosition(drawablePoints, 0, 1, startVector, endVector, startVector);
    CalculateScreenPosition(drawablePoints, 0, 2, endVector, endVector, startVector);
    CalculateScreenPosition(drawablePoints, 0, 3, endVector, startVector, startVector);

    CalculateScreenPosition(drawablePoints, 1, 0, startVector, startVector, endVector);
    CalculateScreenPosition(drawablePoints, 1, 1, startVector, endVector, endVector);
    CalculateScreenPosition(drawablePoints, 1, 2, endVector, endVector, endVector);
    CalculateScreenPosition(drawablePoints, 1, 3, endVector, startVector, endVector);

    // Draw lines if you need
    if (colorLine.has_value())
    {
        DrawLine(drawablePoints, 0, 0, 0, 1, colorLine.value());
        DrawLine(drawablePoints, 0, 1, 0, 2, colorLine.value());
        DrawLine(drawablePoints, 0, 2, 0, 3, colorLine.value());
        DrawLine(drawablePoints, 0, 3, 0, 0, colorLine.value());

        DrawLine(drawablePoints, 1, 0, 1, 1, colorLine.value());
        DrawLine(drawablePoints, 1, 1, 1, 2, colorLine.value());
        DrawLine(drawablePoints, 1, 2, 1, 3, colorLine.value());
        DrawLine(drawablePoints, 1, 3, 1, 0, colorLine.value());

        DrawLine(drawablePoints, 0, 0, 1, 0, colorLine.value());
        DrawLine(drawablePoints, 0, 1, 1, 1, colorLine.value());
        DrawLine(drawablePoints, 0, 2, 1, 2, colorLine.value());
        DrawLine(drawablePoints, 0, 3, 1, 3, colorLine.value());
    }

    // Draw sides if you need
    if (colorSide.has_value())
    {
        DrawSide(drawablePoints, 0, 0, 0, 1, 0, 2, 0, 3, colorSide.value());
        DrawSide(drawablePoints, 1, 0, 1, 1, 1, 2, 1, 3, colorSide.value());

        DrawSide(drawablePoints, 0, 0, 0, 1, 1, 1, 1, 0, colorSide.value());
        DrawSide(drawablePoints, 0, 1, 0, 2, 1, 2, 1, 1, colorSide.value());
        DrawSide(drawablePoints, 0, 2, 0, 3, 1, 3, 1, 2, colorSide.value());
        DrawSide(drawablePoints, 0, 3, 0, 0, 1, 0, 1, 3, colorSide.value());
    }
}

void D3Draw::Draw3DCircle(const Vector3f& startPos, float radius, uint8_t points, color_t color)
{
    weak_assert(radius > 0);
    weak_assert(points > 3); // minimal points count

    Vector3f lastScreenPosition;

    constexpr float maxAngle = M_PI * 2;
    const float step = maxAngle / static_cast<float>(points);

    for (uint8_t i{}; i < points; ++i) {
        const float angle = std::min(step * static_cast<float>(i), maxAngle);
        const Vector3f worldPosition = {
                radius * cos(angle) + startPos.x,
                radius * sin(angle) + startPos.y,
                startPos.z
        };

        Vector3f screenPosition;
        if (!GameUtils::IsOnScreen(worldPosition) || !GameUtils::WorldToScreen(worldPosition, screenPosition))
        {
            lastScreenPosition = 0;
            continue;
        }

        if (!lastScreenPosition.isZero()) {
            auto drawableLastScreenPosition = lastScreenPosition.convert<uint16_t>();
            auto drawableScreenPosition = screenPosition.convert<uint16_t>();

            g_pRender->DrawLine(static_cast<int16_t>(drawableLastScreenPosition.x), static_cast<int16_t>(drawableLastScreenPosition.y),
                                static_cast<int16_t>(drawableScreenPosition.x), static_cast<int16_t>(drawableScreenPosition.y),
                                color);
        }
        lastScreenPosition = screenPosition;
    }
}