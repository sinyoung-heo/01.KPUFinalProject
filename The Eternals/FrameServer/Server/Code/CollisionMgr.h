#pragma once

class CCollisionMgr
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr(void);
	virtual ~CCollisionMgr(void);

private:
	// Line Collision
	bool CheckIntersectPoint(_vec2& AP1, _vec2& AP2, _vec2& BP1, _vec2& BP2, _vec2* IP);
	
public:
	bool Is_DeadReckoning(_vec3& vPos, _vec3& vDir, _vec2* vResult, char naviType);
	bool Is_Arrive(const _vec3& _vStart, const _vec3& _vEnd);
	bool Is_InMoveLimit(const _vec3& _vStart, const _vec3& _vEnd);
public:
	void Release();

};

