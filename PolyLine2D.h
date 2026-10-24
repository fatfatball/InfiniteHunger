#pragma once
#include "BaseGameObject.h"
#include "List.h"
#include "Vector2.h"
#include "SmallStructs.h"

class PolyLine2D : public BaseGameObject {
private:
	List<Vector2> screenPoints; // only for rendering, update every visual frame
	int thicknessPixel; // calculated by thickness in OnStart, radius of circle

public:
	List<Vector2> points; // store the vertices, in world axis
	float thickness; // global axis
	ColorRGBA lineColor;

	PolyLine2D(GamesEngineeringBase::Window& canvas);
	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;

	void UpdateRenderState(const float& alpha, const Camera& camera) override;
	// in this class, these color sampling methods only returns lineColor
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;

	bool DepthTest(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile) 
		const override;
	void RenderSelf(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile)
		const override;

	void SetThickness(const float& val) {
		thickness = val;
		thicknessPixel = thickness / (float)FOV * (float)WINDOW_HEIGHT / 2.f;
	}

private:
	template<typename Plot>
	void DrawLineBresenham(const Vector2& a, const Vector2& b, const ScreenAABB& tile, int raduis, Plot&& plot) const;

	template<typename Plot>
	void DrawCricleMidpoint(int x, int y, const ScreenAABB& tile, int raduis,Plot&& plot) const;

	template<typename Plot>
	void DrawCross(int x, int y, const ScreenAABB& tile, int raduis, Plot&& plot) const;

	template<typename Plot>
	void DrawThickLine(const Vector2& a, const Vector2& b,
		const ScreenAABB& tile, int radius, Plot&& plot) const;
};
