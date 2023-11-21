#pragma once
#include <cmath>
#define M_PI 3.141592
using namespace std;

template<typename T>
class Vector2D {
public:
	struct { T x, y; };

	Vector2D() {}
	Vector2D(const T& x, const T& y) : x(x), y(y) {}
	Vector2D(const Vector2D<T>& other) : x(other.x), y(other.y) {}
	~Vector2D() {}
	Vector2D<T>& operator=(const Vector2D<T>& other) {
		x = other.x;
		y = other.y;
		return *this;
	}
	Vector2D<T> operator+(const Vector2D<T>& other) const {
		return Vector2D<T>(x + other.x, y + other.y);
	}
	Vector2D<T> operator-(const Vector2D<T>& other) const {
		return Vector2D<T>(x - other.x, y - other.y);
	}
	Vector2D<T> operator*(const T& s) const {
		return Vector2D<T>(x * s, y * s);
	}
	Vector2D<T> operator/(const T& s) const {
		return Vector2D<T>(x / s, y / s);
	}
	Vector2D<T> operator-() {
		return Vector2D<T>(-x, -y);
	}
	Vector2D<T>& operator+=(const Vector2D<T>& other) {
		*this = *this + other;
		return *this;
	}
	Vector2D<T>& operator-=(const Vector2D<T>& other) {
		*this = *this - other;
		return *this;
	}
	Vector2D<T>& operator*=(const T& s) {
		*this = *this * s;
		return *this;
	}
	Vector2D<T>& operator/=(const T& s) {
		*this = *this / s;
		return *this;
	}
	bool operator==(const Vector2D<T>& other) {
		return (x == other.x && y == other.y);
	}
	T GetLenth() const {
		T lenth = sqrt(pow(x, 2) + pow(y, 2));
		return lenth;
	}
	Vector2D<T> Rotate(const T& angle) {
		*this = Vector2D<T>(x * std::cos(angle * M_PI / 180) - y * std::sin(angle * M_PI / 180),
			x * std::sin(angle * M_PI / 180) + y * std::cos(angle * M_PI / 180));
		return *this;
	}
	Vector2D<T> Normalize() {
		return *this /= GetLenth();
	}
	T Dot(const Vector2D<T>& other) const {
		T result = (x * other.x) + (y * other.y);
		return result;
	}
	T GetRadian(const Vector2D<T>& other) const {
		T radian = std::acos(Dot(other) / (GetLenth() * other.GetLenth()));
		return (radian * 180 / M_PI);
	}
};