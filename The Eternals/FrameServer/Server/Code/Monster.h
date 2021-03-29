#pragma once
#include "Obj.h"

class CMonster :
    public CObj
{
	
public:
	CMonster();
	virtual ~CMonster();

public:
	/* SEND PACKET */
	void send_Monster_enter_packet(int to_client);
	void send_Monster_move_packet(int to_client, int ani);
	void send_Monster_NormalAttack(int to_client, int ani);
	void send_Monster_Stat(int to_client);

	void Set_Stop_Attack();
	void Set_Start_Attack();

public:
	int	 Update_Monster(const float& fTimeDelta);
	void Hurt_Monster(const int& p_id, const int& damage);					// ATTACKED BY PLAYER

private:
	/* MONSTER NORMAL */
	void Change_Crab_Animation(const float& fTimeDelta);
	void Change_Monkey_Animation(const float& fTimeDelta);
	void Attack_Crab(const float& fTimeDelta);				// ATTACK PROCESS
	void Chase_Monkey(const float& fTimeDelta);
	void Attack_Monkey(const float& fTimeDelta);

	/* MONSTER ALL */
	void Change_Animation(const float& fTimeDelta);

	void Change_AttackMode();								// STATUS == ATTACK
	void Change_ChaseMode();								// STATUS == CHASE

	void Move_ComeBack(const float& fTimeDelta);			// MOVE PROCESS
	void Move_NormalMonster(const float& fTimeDelta);		// MOVE PROCESS
	void Chase_NormalMonster(const float& fTimeDelta);		// MOVE PROCESS

	bool Is_InAttackRange(const _vec3& vPos, const int& range);

public:
	virtual DWORD Release();

public:
	char			m_monNum		= 0;
	int				m_iHp			= 0;
	int				m_iMaxHp		= 0;
	int				m_iExp			= 0;
	int				m_iAtt			= 0;
	float			m_fSpd			= 0;
	bool			m_bIsComeBack	= false;
	volatile bool	m_bIsAttack		= false;
	volatile bool	m_bIsShortAttack = true;

	int				m_iTargetNum	= -1;
	_vec3			m_vOriPos		= _vec3(0.f);
};

