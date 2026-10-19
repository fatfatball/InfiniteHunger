#pragma once

class Math
{
public:
	template<typename T>
	static T Lerp(const T& a, const T& b, const float& alpha) {
		return a * (1 - alpha) + b * alpha;
	}
	
	static float WrapAngle(const float& angle) {
		return std::remainder<float>(angle, TWO_PI);
	}

	static float AngleDifference(const float& from, const float& to) {
		return WrapAngle(to - from);
	}

	static float LerpAngle(const float& a, const float& b, const float& alpha) {
		float diff = AngleDifference(a, b);
		return WrapAngle(a + diff * alpha);
	}
};

