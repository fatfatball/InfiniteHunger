#pragma once
#include <cmath>
#include <stdexcept>

class Vector2
{
public:
	float x = 0;
	float y = 0;

	Vector2() {
		x = 0;
		y = 0;
	}

	Vector2(const float& x, const float& y) {
		this->x = x;
		this->y = y;
	}

	float& operator[](const int& index) {
		switch (index) {
		case 0: 
			return x;
		case 1:
			return y;
		default:
			throw std::out_of_range("Index out of range for Vector3");
		}
	}

	Vector2 operator+(const Vector2& other) const {
		Vector2 a = Vector2(x, y);
		Vector2 b = Vector2(other.x, other.y);

		a.x += b.x;
		a.y += b.y;
		return a;
	}

	Vector2 operator-(const Vector2& other) const {
		Vector2 a = Vector2(x, y);
		Vector2 b = Vector2(other.x, other.y);

		a.x -= b.x;
		a.y -= b.y;
		return a;
	}

	template<typename T>
	Vector2 operator*(const T& val) const {
		return Vector2(x * val, y * val);
	}

	template<typename T>
	Vector2 operator/(const T& val) const {
		return Vector2(x / val, y / val);
	}

	float Length() const {
		return std::sqrt(x * x + y * y);
	}

	Vector2 Normalized() const {
		Vector2 res = Vector2(x, y);
		float l = res.Length();
		if (l < 1e-4f) {
			return res;
		}
		res.x /= l;
		res.y /= l;
		return res;
	}

	float DotProduct(const Vector2& other) const {
		return x * other.x + y * other.y;
	}

	// this X other, sign is the direction
	float CrossProduct(const Vector2& other) const{
		return x * other.y - other.x * y;
	}
};