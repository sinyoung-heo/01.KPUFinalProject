#pragma once
#include "Obj.h"
class CMonster :
    public CObj
{
public:
	CMonster();
	virtual ~CMonster();

public:
	void Set_Stop_Attack();
	void Set_Start_Attack();

	int	 Update_Monster(const float& fTimeDelta);
	void Hurt_Monster(const int& damage);				// ATTACKED BY PLAYER

private:
	void Change_Animation(const float& fTimeDelta);
	void Change_AttackMode();							// STATUS == ATTACK

	void Move_ComeBack(const float& fTimeDelta);		// MOVE PROCESS
	void Move_NormalMonster(const float& fTimeDelta);	// MOVE PROCESS
	void Move_ChaseMonster(const float& fTimeDelta);	// MOVE PROCESS
	void Attack_Monster(const float& fTimeDelta);		// ATTACK PROCESS

public:
	/* SEND PACKET */
	void send_Monster_enter_packet(int to_client);
	void send_Monster_move_packet(int to_client);		
	void send_Monster_NormalAttack(int to_client);

	void send_Monster_Stat(int to_client);

	virtual DWORD Release();

public:
	char			m_monNum;
	int				m_iHp			= 0;
	int				m_iMaxHp		= 0;
	int				m_iExp			= 0;
	int				m_iAtt			= 0;
	float			m_fSpd			= 0;
	bool			m_bIsComeBack	= false;
	volatile bool	m_bIsAttack		= false;

	int				targetNum		= -1;
	int				m_AnimIdx		= 0;
	_vec3			m_vOriPos		= _vec3(0.f);
};

