#pragma once
#include "Include.h"
#include "Base.h"

class CServerMath : public Engine::CBase
{
	DECLARE_SINGLETON(CServerMath)

private:
	explicit CServerMath();
	virtual ~CServerMath() = default;

public:
	/* ¼¼Á¦°ö */
	float pow_square(float t);
	/* Á¦°ö */
	float square(float t);

	bool Is_Arrive_Point(const _vec3& _vStart, const _vec3& _vEnd);
private:
	virtual void Free();

};

