#pragma once

class CCollisionMgr
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr(void);
	virtual ~CCollisionMgr(void);

public:
	// Line Collision
	bool CheckIntersectPoint(_vec2& AP1, _vec2& AP2, _vec2& BP1, _vec2& BP2, _vec2* IP);
	
public:
	void Is_Collision_NaviLine(_vec3& vPos, _vec3& vDir, _vec2* vResult);

public:
	void Release();

};

