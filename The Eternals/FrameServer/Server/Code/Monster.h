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
	/* SEND PACKET */
	void send_Monster_enter_packet(int to_client);
	void send_Monster_move_packet(int to_client);							

	virtual DWORD Release();

public:
	char	m_monNum;
	int		Hp			= 0;
	int		maxHp		= 0;
	int		targetNum	= -1;
	int		m_AnimIdx	= 0;
	STATUS	pre_state	= STATUS::ST_END;
};

