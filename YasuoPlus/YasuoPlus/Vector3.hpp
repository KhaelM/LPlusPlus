#pragma once

#include <Windows.h>
#include <cmath>

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3()
	{
	}

	explicit Vector3(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Set(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Zero()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	float operator[](int index) const
	{
		return (&x)[index];
	}

	float& operator[](int index)
	{
		return (&x)[index];
	}

	Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	float operator*(Vector3& a) const
	{
		return ((x * a.x) + (y * a.y) + (z * a.z));
	}

	Vector3 operator*(const float a) const
	{
		return Vector3(x * a, y * a, z * a);
	}

	Vector3 operator/(const float a) const
	{
		float inva = 1.f / a;
		return Vector3(x * inva, y * inva, z * inva);
	}

	Vector3 operator+(const Vector3& a) const
	{
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator-(const Vector3& a) const
	{
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	Vector3& operator+=(const Vector3& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	Vector3& operator/=(const Vector3& a)
	{
		x /= a.x;
		y /= a.y;
		z /= a.z;
		return *this;
	}

	Vector3& operator/=(const float a)
	{
		x /= a;
		y /= a;
		z /= a;
		return *this;
	}

	Vector3& operator*=(const float a)
	{
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	friend Vector3 operator*(const float a, const Vector3 b)
	{
		return Vector3(b.x * a, b.y * a, b.z * a);
	}

	bool operator==(const Vector3& a) const
	{
		return Compare(a) == true;
	}

	bool operator!=(const Vector3& a) const
	{
		return Compare(a) == false;
	}

	bool Compare(const Vector3& a) const
	{
		return ((x == a.x) && (y == a.y) && (z == a.z));
	}

	Vector3& VectorNormalize()
	{
		float len = Length();

		*this /= len;

		return *this;
	}

	float Length()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float Length2D()
	{
		return sqrtf(x * x + z * z);
	}

	float DotProduct(const Vector3& vecOther)
	{
		return ((x * vecOther.x) + (y * vecOther.y) + (z * vecOther.z));
	}
};