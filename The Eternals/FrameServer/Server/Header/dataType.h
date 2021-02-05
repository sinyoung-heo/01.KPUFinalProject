#pragma once
#include <iostream>

typedef struct tagVector2
{
public:
	explicit tagVector2()
		:x(0.f), y(0.f)
	{}
	explicit tagVector2(const float& _x, const float& _y)
		: x(_x), y(_y)
	{}
	explicit tagVector2(const float& num)
		: x(num), y(num)
	{}
	explicit tagVector2(const tagVector2& vec)
		: x(vec.x), y(vec.y)
	{}

	/* ===============================================¿¬»êÀÚ ¿À¹ö·Îµù=============================================== */
	// µ¡¼À
	tagVector2& operator+(const tagVector2& vec)
	{
		return tagVector2(this->x + vec.x, this->y + vec.y);
	}

	const tagVector2& operator+(const tagVector2& vec) const
	{
		return tagVector2(this->x + vec.x, this->y + vec.y);
	}

	// »¬¼À
	tagVector2& operator-(const tagVector2& vec)
	{
		return tagVector2(this->x - vec.x, this->y - vec.y);
	}

	const tagVector2& operator-(const tagVector2& vec) const
	{
		return tagVector2(this->x - vec.x, this->y - vec.y);
	}

	// °ö¼À
	tagVector2& operator*(const tagVector2& vec)
	{
		return tagVector2(this->x * vec.x, this->y * vec.y);
	}

	tagVector2& operator*(const int& num)
	{
		return tagVector2(this->x * (float)num, this->y * (float)num);
	}

	tagVector2& operator*(const float& num)
	{
		return tagVector2(this->x * num, this->y * num);
	}

	tagVector2& operator*(const double& num)
	{
		return tagVector2(this->x * (float)num, this->y * (float)num);
	}

	const tagVector2& operator*(const tagVector2& vec) const
	{
		return tagVector2(this->x * vec.x, this->y * vec.y);
	}

	const tagVector2& operator*(const int& num) const
	{
		return tagVector2(this->x * (float)num, this->y * (float)num);
	}

	const tagVector2& operator*(const float& num) const
	{
		return tagVector2(this->x * num, this->y * num);
	}

	const tagVector2& operator*(const double& num) const
	{
		return tagVector2(this->x * (float)num, this->y * (float)num);
	}

	friend tagVector2& operator*(const int& num, tagVector2& vec)
	{
		return tagVector2(vec.x * (float)num, vec.y * (float)num);
	}

	friend tagVector2& operator*(const float& num, tagVector2& vec)
	{
		return tagVector2(vec.x * num, vec.y * num);
	}

	friend tagVector2& operator*(const double& num, tagVector2& vec)
	{
		return tagVector2(vec.x * (float)num, vec.y * (float)num);
	}

	// ³ª´°¼À
	tagVector2& operator/(const int& num)
	{
		return tagVector2(this->x / (float)num, this->y / (float)num);
	}

	tagVector2& operator/(const float& num)
	{
		return tagVector2(this->x / num, this->y / num);
	}

	tagVector2& operator/(const double& num)
	{
		return tagVector2(this->x / (float)num, this->y / (float)num);
	}

	const tagVector2& operator/(const int& num) const
	{
		return tagVector2(this->x / (float)num, this->y / (float)num);
	}

	const tagVector2& operator/(const float& num) const
	{
		return tagVector2(this->x / num, this->y / num);
	}

	const tagVector2& operator/(const double& num) const
	{
		return tagVector2(this->x / (float)num, this->y / (float)num);
	}

	friend tagVector2& operator/(const int& num, tagVector2& vec)
	{
		return tagVector2(vec.x / (float)num, vec.y / (float)num);
	}

	friend tagVector2& operator/(const float& num, tagVector2& vec)
	{
		return tagVector2(vec.x / num, vec.y / num);
	}

	friend tagVector2& operator/(const double& num, tagVector2& vec)
	{
		return tagVector2(vec.x / (float)num, vec.y / (float)num);
	}

	// Ãà¾à ¿¬»êÀÚ
	tagVector2& operator+=(const tagVector2& vec)
	{
		return (*this) = (*this) + vec;
	}
	tagVector2& operator-=(const tagVector2& vec)
	{
		return (*this) = (*this) - vec;
	}
	tagVector2& operator*=(const tagVector2& vec)
	{
		return (*this) = (*this) * vec;
	}
	tagVector2& operator*=(const int& num)
	{
		return (*this) = (*this) * (float)num;
	}
	tagVector2& operator*=(const float& num)
	{
		return (*this) = (*this) * num;
	}
	tagVector2& operator*=(const double& num)
	{
		return (*this) = (*this) * (float)num;
	}

	// ºñ±³¿¬»êÀÚ
	bool operator==(const tagVector2& vec)
	{
		if (this->x != vec.x) return false;
		if (this->y != vec.y) return false;

		return true;
	}

	bool operator!=(const tagVector2& vec)
	{
		if (this->x != vec.x) return true;
		if (this->y != vec.y) return true;
		
		return false;
	}

	/* ===============================================Function=============================================== */
	void Normalize()
	{
		// normalize = v / |v|
		float scalar = sqrtf((this->x * this->x) + (this->y * this->y));
		this->x /= scalar;
		this->y /= scalar;
	}

	float Dot(const tagVector2& vec)
	{
		return ((this->x * vec.x) + (this->y * vec.y));
	}

	float Length()
	{
		return sqrtf((this->x * this->x) + (this->y * this->y));
	}

	float Distance(const tagVector2& dst)
	{
		return sqrtf((dst.x - this->x) * (dst.x - this->x) + (dst.y - this->y) * (dst.y - this->y));
	}

	float Angle(tagVector2& vec)
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
	float x, y;

}_vec2;

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

	tagVector3* Cross(tagVector3* target, tagVector3* v1, tagVector3* v2)
	{
		target->x = (v1->y * v2->z) - (v1->z * v2->y);
		target->y = (v1->z * v2->x) - (v1->x * v2->z);
		target->z = (v1->x * v2->y) - (v1->y * v2->x);
		return target;
	}
	
public:
	float x, y, z;

}_vec3;

typedef struct tagVector4
{
public:
	explicit tagVector4()
		:x(0.f), y(0.f), z(0.f), w(0.f)
	{}
	explicit tagVector4(const float& _x, const float& _y, const float& _z, const float& _w)
		: x(_x), y(_y), z(_z), w(0.f)
	{}
	explicit tagVector4(const float& num)
		: x(num), y(num), z(num), w(0.f)
	{}
	explicit tagVector4(const tagVector3& vec)
		: x(vec.x), y(vec.y), z(vec.z), w(0.f)
	{}

	/* ===============================================¿¬»êÀÚ ¿À¹ö·Îµù=============================================== */
	// µ¡¼À
	tagVector4& operator+(const tagVector4& vec)
	{
		return tagVector4(this->x + vec.x, this->y + vec.y, this->z + vec.z, this->w + vec.w);
	}

	const tagVector4& operator+(const tagVector4& vec) const
	{
		return tagVector4(this->x + vec.x, this->y + vec.y, this->z + vec.z, this->w + vec.w);
	}

	// »¬¼À
	tagVector4& operator-(const tagVector4& vec)
	{
		return tagVector4(this->x - vec.x, this->y - vec.y, this->z - vec.z, this->w + vec.w);
	}

	const tagVector4& operator-(const tagVector4& vec) const
	{
		return tagVector4(this->x - vec.x, this->y - vec.y, this->z - vec.z, this->w + vec.w);
	}

	// °ö¼À
	tagVector4& operator*(const tagVector4& vec)
	{
		return tagVector4(this->x * vec.x, this->y * vec.y, this->z * vec.z, this->w + vec.w);
	}

	tagVector4& operator*(const int& num)
	{
		return tagVector4(this->x * (float)num, this->y * (float)num, this->z * (float)num, this->w * (float)num);
	}

	tagVector4& operator*(const float& num)
	{
		return tagVector4(this->x * num, this->y * num, this->z * num, this->w * (float)num);
	}

	tagVector4& operator*(const double& num)
	{
		return tagVector4(this->x * (float)num, this->y * (float)num, this->z * (float)num, this->w * (float)num);
	}

	const tagVector4& operator*(const tagVector4& vec) const
	{
		return tagVector4(this->x * vec.x, this->y * vec.y, this->z * vec.z, this->w * vec.w);
	}

	const tagVector4& operator*(const int& num) const
	{
		return tagVector4(this->x * (float)num, this->y * (float)num, this->z * (float)num, this->w * (float)num);
	}

	const tagVector4& operator*(const float& num) const
	{
		return tagVector4(this->x * num, this->y * num, this->z * num, this->w * num);
	}

	const tagVector4& operator*(const double& num) const
	{
		return tagVector4(this->x * (float)num, this->y * (float)num, this->z * (float)num, this->w * (float)num);
	}

	friend tagVector4& operator*(const int& num, tagVector4& vec)
	{
		return tagVector4(vec.x * (float)num, vec.y * (float)num, vec.z * (float)num, vec.w * (float)num);
	}

	friend tagVector4& operator*(const float& num, tagVector4& vec)
	{
		return tagVector4(vec.x * num, vec.y * num, vec.z * num, vec.w * num);
	}

	friend tagVector4& operator*(const double& num, tagVector4& vec)
	{
		return tagVector4(vec.x * (float)num, vec.y * (float)num, vec.z * (float)num, vec.w * (float)num);
	}

	// ³ª´°¼À
	tagVector4& operator/(const int& num)
	{
		return tagVector4(this->x / (float)num, this->y / (float)num, this->z / (float)num, this->w / (float)num);
	}

	tagVector4& operator/(const float& num)
	{
		return tagVector4(this->x / num, this->y / num, this->z / num, this->w / num);
	}

	tagVector4& operator/(const double& num)
	{
		return tagVector4(this->x / (float)num, this->y / (float)num, this->z / (float)num, this->w / (float)num);
	}

	const tagVector4& operator/(const int& num) const
	{
		return tagVector4(this->x / (float)num, this->y / (float)num, this->z / (float)num, this->w / (float)num);
	}

	const tagVector4& operator/(const float& num) const
	{
		return tagVector4(this->x / num, this->y / num, this->z / num, this->w / num);
	}

	const tagVector4& operator/(const double& num) const
	{
		return tagVector4(this->x / (float)num, this->y / (float)num, this->z / (float)num, this->w / (float)num);
	}

	friend tagVector4& operator/(const int& num, tagVector4& vec)
	{
		return tagVector4(vec.x / (float)num, vec.y / (float)num, vec.z / (float)num, vec.w / (float)num);
	}

	friend tagVector4& operator/(const float& num, tagVector4& vec)
	{
		return tagVector4(vec.x / num, vec.y / num, vec.z / num, vec.w / num);
	}

	friend tagVector4& operator/(const double& num, tagVector4& vec)
	{
		return tagVector4(vec.x / (float)num, vec.y / (float)num, vec.z / (float)num, vec.w / (float)num);
	}

	// Ãà¾à ¿¬»êÀÚ
	tagVector4& operator+=(const tagVector4& vec)
	{
		return (*this) = (*this) + vec;
	}
	tagVector4& operator-=(const tagVector4& vec)
	{
		return (*this) = (*this) - vec;
	}
	tagVector4& operator*=(const tagVector4& vec)
	{
		return (*this) = (*this) * vec;
	}
	tagVector4& operator*=(const int& num)
	{
		return (*this) = (*this) * (float)num;
	}
	tagVector4& operator*=(const float& num)
	{
		return (*this) = (*this) * num;
	}
	tagVector4& operator*=(const double& num)
	{
		return (*this) = (*this) * (float)num;
	}

	// ºñ±³¿¬»êÀÚ
	bool operator==(const tagVector4& vec)
	{
		if (this->x != vec.x) return false;
		if (this->y != vec.y) return false;
		if (this->z != vec.z) return false;
		if (this->w != vec.w) return false;
		return true;
	}

	bool operator!=(const tagVector4& vec)
	{
		if (this->x != vec.x) return true;
		if (this->y != vec.y) return true;
		if (this->z != vec.z) return true;
		if (this->w != vec.w) return false;
		return false;
	}

	/* ===============================================Function=============================================== */
	void Normalize()
	{
		// normalize = v / |v|
		float scalar = sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w));
		this->x /= scalar;
		this->y /= scalar;
		this->z /= scalar;
		this->w /= scalar;
	}

	float Dot(const tagVector4& vec)
	{
		return ((this->x * vec.x) + (this->y * vec.y) + (this->z * vec.z) + (this->w * vec.w));
	}

	float Length()
	{
		return sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z) + (this->w * this->w));
	}

	float Distance(const tagVector4& dst)
	{
		return sqrtf((dst.x - this->x) * (dst.x - this->x) + (dst.y - this->y) * (dst.y - this->y) + (dst.z - this->z) * (dst.z - this->z) + (dst.w - this->w) * (dst.w - this->w));
	}

	float Angle(tagVector4& vec)
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
	float x, y, z, w;

}_vec4,_plane;

