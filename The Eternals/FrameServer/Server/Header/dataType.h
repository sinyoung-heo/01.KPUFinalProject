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

public:
	float x, y, z;



}_vec3;


