#include "stdafx.h"
#include "ServerMath.h"

IMPLEMENT_SINGLETON(CServerMath)

CServerMath::CServerMath()
{
}

float CServerMath::pow_square(float t)
{
	return (float)(t * t * t);
}

float CServerMath::square(float t)
{
	return (float)(t * t);
}

bool CServerMath::Is_Arrive_Point(const _vec3& _vStart, const _vec3& _vEnd)
{
	float dist = (_vEnd.x - _vStart.x) * (_vEnd.x - _vStart.x);
	dist += (_vEnd.y - _vStart.y) * (_vEnd.y - _vStart.y);
	dist += (_vEnd.z - _vStart.z) * (_vEnd.z - _vStart.z);

	return dist <= 10.f;
}

void CServerMath::Free()
{
}
