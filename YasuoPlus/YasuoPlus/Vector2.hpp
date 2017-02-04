#pragma once

#include <Windows.h>
#include <cmath>

struct Vector2
{
	float x;
	float y;

	Vector2()
	{
	}

	explicit Vector2(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	void Set(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	void Zero()
	{
		this->x = 0;
		this->y = 0;
	}

	float operator[](int index) const
	{
		return (&x)[index];
	}

	float& operator[](int index)
	{
		return (&x)[index];
	}

	Vector2 operator-() const
	{
		return Vector2(-x, -y);
	}

	float operator*(Vector2& a) const
	{
		return ((x * a.x) + (y * a.y));
	}

	Vector2 operator*(const float a) const
	{
		return Vector2(x * a, y * a);
	}

	Vector2 operator/(const float a) const
	{
		float inva = 1.f / a;
		return Vector2(x * inva, y * inva);
	}

	Vector2 operator+(const Vector2& a) const
	{
		return Vector2(x + a.x, y + a.y);
	}

	Vector2 operator-(const Vector2& a) const
	{
		return Vector2(x - a.x, y - a.y);
	}

	Vector2& operator+=(const Vector2& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}

	Vector2& operator/=(const Vector2& a)
	{
		x /= a.x;
		y /= a.y;
		return *this;
	}

	Vector2& operator/=(const float a)
	{
		x /= a;
		y /= a;
		return *this;
	}

	Vector2& operator*=(const float a)
	{
		x *= a;
		y *= a;
		return *this;
	}

	friend Vector2 operator*(const float a, const Vector2 b)
	{
		return Vector2(b.x * a, b.y * a);
	}

	bool operator==(const Vector2& a) const
	{
		return Compare(a) == true;
	}

	bool operator!=(const Vector2& a) const
	{
		return Compare(a) == false;
	}

	bool Compare(const Vector2& a) const
	{
		return ((x == a.x) && (y == a.y));
	}

	Vector2& VectorNormalize()
	{
		float len = Length();

		*this /= len;

		return *this;
	}

	float Length()
	{
		return sqrtf(x * x + y * y);
	}

	float DotProduct(const Vector2& vecOther)
	{
		return ((x * vecOther.x) + (y * vecOther.y));
	}
};