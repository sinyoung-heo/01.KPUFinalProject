#pragma once
#include <iostream>

typedef struct tagVector3
{
public:
	explicit tagVector3() 
		:x(0.f), y(0.f), z(0.f) 
	{}
	explicit tagVector3(const float& _x, const float& _y, const float& _z)
		: x(_x), y(_y), z(_z)
	{}
	explicit tagVector3(const float& num)
		: x(num), y(num), z(num) 
	{}
	explicit tagVector3(const tagVector3& vec) 
		: x(vec.x), y(vec.y), z(vec.z) 
	{}
	
	/* ===============================================¿¬»êÀÚ ¿À¹ö·Îµù=============================================== */
	// µ¡¼À
	tagVector3& operator+(const tagVector3& vec) 
	{ 
		return tagVector3(this->x + vec.x, this->y + vec.y, this->z + vec.z);
	}

	const tagVector3& operator+(const tagVector3& vec) const 
	{ 
		return tagVector3(this->x + vec.x, this->y + vec.y, this->z + vec.z);
	}

	// »¬¼À
	tagVector3& operator-(const tagVector3& vec)
	{
		return tagVector3(this->x - vec.x, this->y - vec.y, this->z - vec.z);
	}

	const tagVector3& operator-(const tagVector3& vec) const
	{
		return tagVector3(this->x - vec.x, this->y - vec.y, this->z - vec.z);
	}

	// °ö¼À
	tagVector3& operator*(const tagVector3& vec)
	{
		return tagVector3(this->x * vec.x, this->y * vec.y, this->z * vec.z);
	}

	tagVector3& operator*(const int& num)
	{
		return tagVector3(this->x * (float)num, this->y * (float)num, this->z * (float)num);
	}

	tagVector3& operator*(const float& num)
	{
		return tagVector3(this->x * num, this->y * num, this->z * num);
	}

	tagVector3& operator*(const double& num)
	{
		return tagVector3(this->x * (float)num, this->y * (float)num, this->z * (float)num);
	}

	const tagVector3& operator*(const tagVector3& vec) const
	{
		return tagVector3(this->x * vec.x, this->y * vec.y, this->z * vec.z);
	}

	const tagVector3& operator*(const int& num) const
	{
		return tagVector3(this->x * (float)num, this->y * (float)num, this->z * (float)num);
	}

	const tagVector3& operator*(const float& num) const
	{
		return tagVector3(this->x * num, this->y * num, this->z * num);
	}

	const tagVector3& operator*(const double& num) const
	{
		return tagVector3(this->x * (float)num, this->y * (float)num, this->z * (float)num);
	}

	friend tagVector3& operator*(const int& num, tagVector3& vec)
	{
		return tagVector3(vec.x * (float)num, vec.y * (float)num, vec.z * (float)num);
	}

	friend tagVector3& operator*(const float& num, tagVector3& vec)
	{
		return tagVector3(vec.x * num, vec.y * num, vec.z * num);
	}

	friend tagVector3& operator*(const double& num, tagVector3& vec)
	{
		return tagVector3(vec.x * (float)num, vec.y * (float)num, vec.z * (float)num);
	}

	// ³ª´°¼À
	tagVector3& operator/(const int& num)
	{
		return tagVector3(this->x / (float)num, this->y / (float)num, this->z / (float)num);
	}

	tagVector3& operator/(const float& num)
	{
		return tagVector3(this->x / num, this->y / num, this->z / num);
	}

	tagVector3& operator/(const double& num)
	{
		return tagVector3(this->x / (float)num, this->y / (float)num, this->z / (float)num);
	}

	const tagVector3& operator/(const int& num) const
	{
		return tagVector3(this->x / (float)num, this->y / (float)num, this->z / (float)num);
	}

	const tagVector3& operator/(const float& num) const
	{
		return tagVector3(this->x / num, this->y / num, this->z / num);
	}

	const tagVector3& operator/(const double& num) const
	{
		return tagVector3(this->x / (float)num, this->y / (float)num, this->z / (float)num);
	}

	friend tagVector3& operator/(const int& num, tagVector3& vec)
	{
		return tagVector3(vec.x / (float)num, vec.y / (float)num, vec.z / (float)num);
	}

	friend tagVector3& operator/(const float& num, tagVector3& vec)
	{
		return tagVector3(vec.x / num, vec.y / num, vec.z / num);
	}

	friend tagVector3& operator/(const double& num, tagVector3& vec)
	{
		return tagVector3(vec.x / (float)num, vec.y / (float)num, vec.z / (float)num);
	}

	// Ãà¾à ¿¬»êÀÚ
	tagVector3& operator+=(const tagVector3& vec)
	{
		return (*this) = (*this) + vec;
	}
	tagVector3& operator-=(const tagVector3& vec)
	{
		return (*this) = (*this) - vec;
	}
	tagVector3& operator*=(const tagVector3& vec)
	{
		return (*this) = (*this) * vec;
	}
	tagVector3& operator*=(const int& num)
	{
		return (*this) = (*this) * (float)num;
	}
	tagVector3& operator*=(const float& num)
	{
		return (*this) = (*this) * num;
	}
	tagVector3& operator*=(const double& num)
	{
		return (*this) = (*this) * (float)num;
	}

	// ºñ±³¿¬»êÀÚ
	bool operator==(const tagVector3& vec)
	{
		if (this->x != vec.x) return false;
		if (this->y != vec.y) return false;
		if (this->z != vec.z) return false;
		return true;
	}

	bool operator!=(const tagVector3& vec)
	{
		if (this->x != vec.x) return true;
		if (this->y != vec.y) return true;
		if (this->z != vec.z) return true;
		return false;
	}

	/* ===============================================Function=============================================== */
	void Normalize()
	{
		// normalize = v / |v|
		float scalar = sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
		this->x /= scalar;
		this->y /= scalar;
		this->z /= scalar;
	}

	float Dot(const tagVector3& vec)
	{
		return ((this->x * vec.x) + (this->y * vec.y) + (this->z * vec.z));
	}

	float Length()
	{
		return sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	}

	float Distance(const tagVector3& dst)
	{
		return sqrtf((dst.x - this->x) * (dst.x - this->x) + (dst.y - this->y) * (dst.y - this->y) + (dst.z - this->z) * (dst.z - this->z));
	}
	
	float Angle(tagVector3& vec)
	{
		// src normalize
		this->Normalize();	

		// dst normalize
		vec.Normalize();
		
		/* µÎ ´ÜÀ§ º¤ÅÍ ³»Àû -> cos¥È °ª µµÃâ */
		float cos = this->Dot(vec);

		/* Radian = acos(cos¥È) */
		float theta = acosf(cos);

		// degree
		return theta * (180.f / (float)PI);
	}
	
public:
	float x, y, z;

}_vec3;


