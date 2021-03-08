#pragma once
#include "Obj.h"
class CMonster :
    public CObj
{
public:
	CMonster();
	virtual ~CMonster();

public:
	int Update_Monster(const float& fTimeDelta);

private:
	void Change_Animation(const float& fTimeDelta);
	void Move_NormalMonster(const float& fTimeDelta);
	void Move_ChaseMonster(const float& fTimeDelta);

public:
	virtual DWORD Release();

public:
	char	m_monNum;
	int		Hp			= 0;
	int		maxHp		= 0;
	int		targetNum	= -1;
	int		m_AnimIdx	= 0;
	STATUS	pre_state	= STATUS::ST_END;

private:
	unordered_set<int> view_list;
	mutex v_lock;
};

