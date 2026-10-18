#pragma once
class Vector3
{
public:
	float x = 0;
	float y = 0;
	float w = 1;

	Vector3() {
		x = 0;
		y = 0;
		w = 1;
	}

	Vector3(const float& x, const float& y, const float& w) {
		this->x = x;
		this->x = y;
		this->x = w;
	}

	float& operator[](const int& index) {
		switch (index) {
		case 0: 
			return x;
		case 1:
			return y;
		case 2:
			return w;
		}
	}

	Vector3 operator+(const Vector3& other) {
		Vector3 a = Vector3(x, y, w);
		if (w != 0) {
			a = Simplified();
		}
		Vector3 b = Vector3(other.x, other.y, other.w);
		if (other.w != 0) {
			b = other.Simplified();
		}

		a.x += b.x;
		a.y += b.y;
		a.w = a.w || b.w;
		return a;
	}

	Vector3 operator-(const Vector3& other) {
		Vector3 a = Vector3(x, y, w);
		if (w != 0) {
			a = Simplified();
		}
		Vector3 b = Vector3(other.x, other.y, other.w);
		if (other.w != 0) {
			b = other.Simplified();
		}

		a.x -= b.x;
		a.y -= b.y;
		a.w = a.w || b.w;
		return a;
	}

	template<typename T>
	Vector3 operator*(const T& val) {
		return Vector3(x * val, y * val, w);
	}

	float Length() const {
		if (w == 0) {
			return sqrt(x * x + y * y);
		}
		else {
			return sqrt(x * x + y * y) / w;
		}
	}

	Vector3 Simplified() const {
		Vector3 res = Vector3(x, y, w);
		if (res.w != 0 && res.w != 1) {
			res.x /= res.w;
			res.y /= res.w;
			res.w = 1;
		}
		return res;
	}

	Vector3 Normalized() const {
		Vector3 res = Simplified();
		float l = res.Length();
		res.x /= l;
		res.y /= l;
		return res;
	}
};

class ColorRGB {
public:
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	ColorRGB() {
		r = 0;
		g = 0;
		b = 0;
	}
};