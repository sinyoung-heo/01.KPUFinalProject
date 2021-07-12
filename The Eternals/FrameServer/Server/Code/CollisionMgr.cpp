#include "stdafx.h"
#include "CollisionMgr.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr(void)
{
}

CCollisionMgr::~CCollisionMgr(void)
{
}

bool CCollisionMgr::CheckIntersectPoint(_vec2& AP1, _vec2& AP2, _vec2& BP1, _vec2& BP2, _vec2* IP)
{
	double t;
	double s;
	double under = (BP2.y - BP1.y) * (AP2.x - AP1.x) - (BP2.x - BP1.x) * (AP2.y - AP1.y);
	if (under == 0)
		return false;

	double _t = (BP2.x - BP1.x) * (AP1.y - BP1.y) - (BP2.y - BP1.y) * (AP1.x - BP1.x);
	double _s = (AP2.x - AP1.x) * (AP1.y - BP1.y) - (AP2.y - AP1.y) * (AP1.x - BP1.x);

	t = _t / under;
	s = _s / under;

	if (t < 0.0 || t>1.0 || s < 0.0 || s>1.0)
		return false;
	if (_t == 0 && _s == 0)
		return false;

	IP->x = AP1.x + t * (double)(AP2.x - AP1.x);
	IP->y = AP1.y + t * (double)(AP2.y - AP1.y);

	return true;
}

bool CCollisionMgr::Is_DeadReckoning(_vec3& vPos, _vec3& vDir, _vec2* vResult, char naviType)
{
	_vec2 vStart = _vec2(vPos.x, vPos.z);
	_vec2 vEnd = vStart + _vec2(vDir.x * 256.f, vDir.z * 256.f);
	vector<_vec2> vecPoint;

	/* NaviMesh의 모든 Cell 검사 */
	switch (naviType)
	{
	case STAGE_VELIKA:
	{
		for (auto& c : CNaviMesh::GetInstance()->Get_CellList())
		{
			/* 각 Cell의 Line 검사 */
			for (int i = 0; i < 3; ++i)
			{
				if (CheckIntersectPoint(vStart, vEnd, c->Get_Line(i).Get_Point()[0], c->Get_Line(i).Get_Point()[1], vResult))
					vecPoint.emplace_back(vResult->x, vResult->y);			
			}
		}
	}
	break;

	case STAGE_BEACH:
	{
		for (auto& c : CNaviMesh_Beach::GetInstance()->Get_CellList())
		{
			/* 각 Cell의 Line 검사 */
			for (int i = 0; i < 3; ++i)
			{
				if (CheckIntersectPoint(vStart, vEnd, c->Get_Line(i).Get_Point()[0], c->Get_Line(i).Get_Point()[1], vResult))
					vecPoint.emplace_back(vResult->x, vResult->y);		
			}
		}
	}
	break;

	case STAGE_WINTER:
	{
		for (auto& c : CNaviMesh_Winter::GetInstance()->Get_CellList())
		{
			/* 각 Cell의 Line 검사 */
			for (int i = 0; i < 3; ++i)
			{
				if (CheckIntersectPoint(vStart, vEnd, c->Get_Line(i).Get_Point()[0], c->Get_Line(i).Get_Point()[1], vResult))
					vecPoint.emplace_back(vResult->x, vResult->y);	
			}
		}
	}
	break;
	}

	float ret = 0.f;
	bool bIsRet = false;
	for (auto& r : vecPoint)
	{
		float dist = (vPos.x - r.x) * (vPos.x - r.x);
		dist += (vPos.z - r.y) * (vPos.z - r.y);

		if (ret < dist)
		{
			bIsRet = true;
			ret = dist;
			*vResult = r;
		}
	}

	if (bIsRet) 
		return true;	
	else 
		return false;
}

bool CCollisionMgr::Is_Arrive(const _vec3& _vStart, const _vec3& _vEnd)
{
	float dist = (_vEnd.x - _vStart.x) * (_vEnd.x - _vStart.x);
	dist += (_vEnd.y - _vStart.y) * (_vEnd.y - _vStart.y);
	dist += (_vEnd.z - _vStart.z) * (_vEnd.z - _vStart.z);

	return dist <= 1.f;
}

bool CCollisionMgr::Is_InMoveLimit(const _vec3& _vStart, const _vec3& _vEnd, const int& limit)
{
	float dist = (_vEnd.x - _vStart.x) * (_vEnd.x - _vStart.x);
	dist += (_vEnd.y - _vStart.y) * (_vEnd.y - _vStart.y);
	dist += (_vEnd.z - _vStart.z) * (_vEnd.z - _vStart.z);

	return dist <= (limit * limit);
}

void CCollisionMgr::Release()
{
}
