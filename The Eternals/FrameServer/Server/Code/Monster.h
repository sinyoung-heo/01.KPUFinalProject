#pragma once
#include "Obj.h"

class CMonster :
    public CObj
{
public:
	CMonster();
	virtual ~CMonster();

public:
	void	Set_AnimDuration(double arr[]);
	void	Set_NumAnimation(const _uint& num) { m_uiNumAniIndex = num; }
	void	Set_AnimationKey(const _uint& uiAniKey);

	void	Set_Stop_Attack(chrono::seconds t = 3s);
	void	Set_Start_Attack();

public:
	int		Update_Monster(const float& fTimeDelta);
	void	Hurt_Monster(const int& p_id, const int& damage);	// ATTACKED BY PLAYER
	
	/* MONSTER STATUS CHANGE */
	void	Change_AttackMode();								// STATUS == ATTACK
	void	Change_ChaseMode();									// STATUS == CHASE

private:
	/* MONSTER ALL FUNC */
	void	Change_Animation(const float& fTimeDelta);
	void	Move_NormalMonster(const float& fTimeDelta);		// MOVE PROCESS

	/* MONSTER INDIVIDUAL FUNC */
	void	Change_Crab_Animation(const float& fTimeDelta);
	void	Chase_Crab(const float& fTimeDelta);				
	void	Attack_Crab(const float& fTimeDelta);	

	void	Change_Monkey_Animation(const float& fTimeDelta);
	void	Chase_Monkey(const float& fTimeDelta);
	void	Attack_Monkey(const float& fTimeDelta);

	void	Change_Cloder_Animation(const float& fTimeDelta);
	void	Chase_Cloder(const float& fTimeDelta);
	void	Attack_Cloder(const float& fTimeDelta);

	void	Change_DrownedSailor_Animation(const float& fTimeDelta);
	void	Chase_DrownedSailor(const float& fTimeDelta);
	void	Attack_DrownedSailor(const float& fTimeDelta);
	void	Rush_DrownedSailor(const float& fTimeDelta);

	void	Change_GiantBeetle_Animation(const float& fTimeDelta);
	void	Chase_GiantBeetle(const float& fTimeDelta);
	void	Attack_GiantBeetle(const float& fTimeDelta);
	void	Rush_GiantBeetle(const float& fTimeDelta);

	void	Change_GiantMonkey_Animation(const float& fTimeDelta);
	void	Chase_GiantMonkey(const float& fTimeDelta);
	void	Attack_GiantMonkey(const float& fTimeDelta);

private:
	void	Play_Animation(float fTimeDelta);	
	bool	Is_AnimationSetEnd(const float& fTimeDelta);

	float	Calculate_TargetDist(const _vec3& vPos);

public:
	virtual DWORD Release();

	/* SEND PACKET */
	void	send_Monster_enter_packet(int to_client);
	void	send_Monster_move_packet(int to_client, int ani);
	void	send_Monster_NormalAttack(int to_client, int ani);
	void	send_Monster_RushAttack(int to_client, int ani);
	void	send_Monster_Stat(int to_client);

public:
	char			m_monNum				= 0;
	int				m_iHp					= 0;
	int				m_iMaxHp				= 0;
	int				m_iExp					= 0;
	int				m_iAtt					= 0;
	float			m_fSpd					= 0;
	
	volatile bool	m_bIsAttack				= false;
	volatile bool	m_bIsShortAttack		= true;		// 근거리 공격
	bool			m_bIsRushAttack			= false;

	int				m_iTargetNum			= -1;
	_vec3			m_vOriPos				= _vec3(0.f);
	_vec3			m_vRushPos				= _vec3(0.f);

	/* Animation */
	_uint			m_uiNewAniIndex			= 0;
	_uint			m_uiCurAniIndex			= 0;
	_uint			m_uiNumAniIndex			= 0;
	_uint			m_ui3DMax_NumFrame		= 0;
	_uint			m_ui3DMax_CurFrame		= 0;

	float			m_fAnimationTime		= 0.f;
	float			m_fBlendAnimationTime	= 0.f;
	float			m_fBlendingTime			= 0.f;

	double			m_arrDuration[MAX_ANI]	= { 0 };

	_uint			m_uiAnimIdx				= 0;   // Apply Animation Index
};

