#include "PolyLine2D.h"
#include "Math.h"
#include <cmath>
#include <algorithm>

PolyLine2D::PolyLine2D(GamesEngineeringBase::Window& canvas) : BaseGameObject(canvas) {
	renderOrder = RenderOrder::Second;
	thickness = 10.f;
	lineColor = { 255, 255, 255, 255 };
    thicknessPixel = thickness / (float)FOV * (float)WINDOW_HEIGHT / 2.f;
}

void PolyLine2D::OnStart() {
    thicknessPixel = thickness / (float)FOV * (float)WINDOW_HEIGHT / 2.f;
}

void PolyLine2D::FixedUpdate(const float& dt) {}

void PolyLine2D::VisualUpdate(const float& dt) {}

void PolyLine2D::OnDestroy() {}

void PolyLine2D::UpdateRenderState(const float& alpha, const Camera& camera) {
	renderVisible = points.GetSize() >= 2;
	if (!renderVisible) {
		return;
	}

	screenPoints.Clear();
    float minX = points[0].x;
    float minY = points[0].y;
    float maxX = points[0].x;
    float maxY = points[0].y;
    for (int i = 0; i < points.GetSize(); ++i) {
        Vector2 s = camera.World2ScreenNoClamp(points[i]);
        screenPoints.PushBack(s);

        minX = std::min(minX, s.x);
        minY = std::min(minY, s.y);
        maxX = std::max(maxX, s.x);
        maxY = std::max(maxY, s.y);
    }

    minX = std::clamp(minX - thicknessPixel - 1, 0.f, WINDOW_WIDTH - 1.f);
    minY = std::clamp(minY - thicknessPixel - 1, 0.f, WINDOW_HEIGHT - 1.f);
    maxX = std::clamp(maxX + thicknessPixel + 1, 0.f, WINDOW_WIDTH - 1.f);
    maxY = std::clamp(maxY + thicknessPixel + 1, 0.f, WINDOW_HEIGHT - 1.f);

    renderScreenAABB = ScreenAABB((short)minX, (short)minY, (short)maxX, (short)maxY);
}

ColorRGBA PolyLine2D::GetColorAt(const float& u, const float& v) const {
	return lineColor;
}

unsigned char PolyLine2D::GetAlphaAt(const float& u, const float& v) const {
	return lineColor.a;
}

bool PolyLine2D::DepthTest(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile) const {
    if (!renderVisible || !renderScreenAABB.Intersects(tile)) {
        return false;
    }
    // expand the tile to contain the thickness
    ScreenAABB expandedTile = tile;
    expandedTile.minX -= thicknessPixel;
    expandedTile.minY -= thicknessPixel;
    expandedTile.maxX += thicknessPixel;
    expandedTile.maxY += thicknessPixel;

    auto f = [&](int x, int y, const ScreenAABB tile) {
        if (!tile.IsInside(x, y)) {
            return;
        }
        int index = y * WINDOW_WIDTH + x;
        if (camera.depthGraph[index] < curDepth) {
            return;
        }
        if (lineColor.a == 255) {
            camera.depthGraph[index] = curDepth;
        }
        };

    for (int i = 0; i < screenPoints.GetSize() - 1; ++i) {
        Vector2 a = screenPoints[i];
        Vector2 b = screenPoints[i + 1];
        // ?: add filled circle draw here
        if (!Math::ClipLineToTile(a, b, expandedTile)) {
            continue;
        }
        DrawLineBresenham(a, b, tile, thicknessPixel, f);
    }

    return true;
}

void PolyLine2D::RenderSelf(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile) const {
    if (!renderVisible || !renderScreenAABB.Intersects(tile)) {
        return;
    }

    unsigned char* backBuffer = env.getBackBuffer();
    // expand the tile to contain the thickness
    ScreenAABB expandedTile = tile;
    expandedTile.minX -= thicknessPixel;
    expandedTile.minY -= thicknessPixel;
    expandedTile.maxX += thicknessPixel;
    expandedTile.maxY += thicknessPixel;

    auto f = [&](int x, int y, const ScreenAABB tile) {
        if (!tile.IsInside(x, y)) {
            return;
        }
        int index = y * WINDOW_WIDTH + x;
        if (camera.depthGraph[index] < curDepth) {
            return;
        }
        camera.DrawOnCanvas(env, backBuffer, x, y, lineColor);
        };

    int size = screenPoints.GetSize();
    for (int i = 0; i < size - 1; ++i) {
        Vector2 a = screenPoints[i];
        Vector2 b = screenPoints[i + 1]; 
        // ?: add filled circle draw here
        if (!Math::ClipLineToTile(a, b, expandedTile)) {
            continue;
        }
        DrawLineBresenham(a, b, tile, thicknessPixel, f);
    }
}

template<typename Plot>
void PolyLine2D::DrawLineBresenham(const Vector2& a, const Vector2& b, const ScreenAABB& tile, int raduis, Plot&& plot) const {
    int x0 = std::round(a.x);
    int x1 = std::round(b.x);
    int y0 = std::round(a.y);
    int y1 = std::round(b.y);
    
    int dx = std::abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx + dy;

    while (true) {
        // originally, just plot. here I want thickness so plot a circle here.
        //DrawCricleMidpoint(x0, y0, tile, raduis, plot);
        // circle is not enough. draw a filled circle instead.
        //DrawFilledCricleMidpoint(x0, y0, tile, raduis, plot);
        // maybe an unfilled circle and a cross would be ok
        DrawCricleMidpoint(x0, y0, tile, raduis, plot);
        DrawCross(x0, y0, tile, raduis - 1, plot);

        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

template<typename Plot>
void PolyLine2D::DrawCricleMidpoint(int x, int y, const ScreenAABB& tile, int raduis, Plot&& plot) const {
    int tx = 0;
    int ty = raduis;
    int p = 1 - raduis;
    plot(x + tx, y + ty, tile);
    plot(x - tx, y + ty, tile);
    plot(x + tx, y - ty, tile);
    plot(x - tx, y - ty, tile);
    plot(x + ty, y + tx, tile);
    plot(x - ty, y + tx, tile);
    plot(x + ty, y - tx, tile);
    plot(x - ty, y - tx, tile);

    while (tx < ty) {
        ++tx;
        if (p > 0) {
            --ty;
            p += 2 * (tx - ty) + 1;
        }
        else {
            p += 2 * tx + 1;
        }

        plot(x + tx, y + ty, tile);
        plot(x - tx, y + ty, tile);
        plot(x + tx, y - ty, tile);
        plot(x - tx, y - ty, tile);
        plot(x + ty, y + tx, tile);
        plot(x - ty, y + tx, tile);
        plot(x + ty, y - tx, tile);
        plot(x - ty, y - tx, tile);
    }
}

template<typename Plot>
void PolyLine2D::DrawCross(int x, int y, const ScreenAABB& tile, int raduis, Plot&& plot) const {
    for (int i = -raduis; i <= raduis; ++i) {
        plot(x + i, y, tile);
    }
    for (int i = -raduis; i <= raduis; ++i) {
        plot(x, y + i, tile);
    }
}

template<typename Plot>
void PolyLine2D::DrawThickLine(const Vector2& a, const Vector2& b,
    const ScreenAABB& tile, int radius, Plot&& plot) const {
    Vector2 dir = (b - a).Normalized();
    Vector2 normal = Vector2(-dir.y, dir.x);

    for (int k = -radius; k <= radius; ++k) {
        Vector2 a2 = a + normal * k;
        Vector2 b2 = b + normal * k;
        DrawLineBresenham(a2, b2, tile, radius, plot);
    }
}

template<typename Plot>
void PolyLine2D::DrawFilledCricleMidpoint(int x, int y,
    const ScreenAABB& tile, int raduis, Plot&& plot) const {
    int tx = 0;
    int ty = raduis;
    int p = 1 - raduis;
    DrawHorizontalLine(y + ty, x - tx, x + tx, tile, plot);
    DrawHorizontalLine(y - ty, x - tx, x + tx, tile, plot);
    DrawHorizontalLine(y + tx, x - ty, x + ty, tile, plot);
    DrawHorizontalLine(y - tx, x - ty, x + ty, tile, plot);

    while (tx < ty) {
        ++tx;
        if (p > 0) {
            --ty;
            p += 2 * (tx - ty) + 1;
        }
        else {
            p += 2 * tx + 1;
        }

        DrawHorizontalLine(y + ty, x - tx, x + tx, tile, plot);
        DrawHorizontalLine(y - ty, x - tx, x + tx, tile, plot);
        DrawHorizontalLine(y + tx, x - ty, x + ty, tile, plot);
        DrawHorizontalLine(y - tx, x - ty, x + ty, tile, plot);
    }
}

template<typename Plot>
void PolyLine2D::DrawHorizontalLine(int y, int x1, int x2,
    const ScreenAABB& tile, Plot&& plot) const {
    for (int i = x1; i <= x2; ++i) {
        plot(i, y, tile);
    }
}