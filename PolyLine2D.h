#pragma once
#include "BaseGameObject.h"
#include "List.h"
#include "Vector2.h"
#include "SmallStructs.h"

// this is a specially rendered object. because it might cover huge AABB, but only render few pixels.
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
	// several functions trying to render the line correctly and efficiently
	// the final solution is DrawLineBresenham + DrawCricleMidpoint + DrawCross
	// this method has fault in ends of the segments (**semi-circle** + rectangle + **semi-circle**),
	// so just draw two filled circles at the ends.
	// L: length, T: thickness. O(L*T + T^2).
	// so, when T<<L, this method performs better than Distance Capsule method (O(n^2)).
	// ---
	// but the filled circle is complex to draw, so just use the fault way:
	// DrawLineBresenham + DrawCricleMidpoint + DrawCross

	// basic pixel line drawing function
	template<typename Plot>
	void DrawLineBresenham(const Vector2& a, const Vector2& b, const ScreenAABB& tile, int raduis, Plot&& plot) const;

	// performs well (O(n)), but there are pixels within the line but not drawn.
	template<typename Plot>
	void DrawCricleMidpoint(int x, int y, const ScreenAABB& tile, int raduis,Plot&& plot) const;

	// performs well (O(n)), but the thickness changes in different directions.
	template<typename Plot>
	void DrawCross(int x, int y, const ScreenAABB& tile, int raduis, Plot&& plot) const;

	// like DrawCricleMidpoint, have some faults.
	template<typename Plot>
	void DrawThickLine(const Vector2& a, const Vector2& b,
		const ScreenAABB& tile, int radius, Plot&& plot) const;

	// correct, but performs worse (O(n^2))
	template<typename Plot>
	void DrawFilledCricleMidpoint(int x, int y,
		const ScreenAABB& tile, int raduis, Plot&& plot) const;

	// helper function
	template<typename Plot>
	void DrawHorizontalLine(int y, int x1, int x2,
		const ScreenAABB& tile, Plot&& plot) const;
};
