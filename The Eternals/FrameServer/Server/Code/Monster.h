#pragma once
#include "Obj.h"

class CMonster :
    public CObj
{
	enum ATTACK_DIST
	{
		DIST_SHORT = 0, DIST_MIDDLE, DIST_LONG, DIST_END
	};
public:
	CMonster();
	virtual ~CMonster();

public:
	const bool& Get_Dead() { return m_bIsDead; }

	void	Set_AnimDuration(double arr[]);
	void	Set_NumAnimation(const _uint& num) { m_uiNumAniIndex = num; }
	void	Set_AnimationKey(const _uint& uiAniKey);

	void	Init_AllStatus();
	void	Set_Stop_Attack(chrono::seconds t = 3s);
	void	Set_Start_Attack();
	void	Set_Start_Fight();
	void	Set_Stop_Fight();
	void	Set_Start_Regen(chrono::seconds t = 3s);
	void	Set_Finish_Regen();
	void	Set_Start_Reaction();
	void	Set_Stop_Reaction(chrono::seconds t = 3s);

	void	active_monster();									// 해당 Monster의 STATUS = ST_ACTIVE
	void	nonActive_monster();								// 해당 Monster의 STATUS = ST_NONACTIVE

public:
	void	Ready_Monster(const _vec3& pos, const _vec3& angle, const char& type, const char& num, const char& naviType, const int& hp, const int& att, const int& exp, const float& spd);
	int		Update_Monster(const float& fTimeDelta);
	void	Hurt_Monster(const int& p_id, const int& damage, const char& affect);	// ATTACKED BY PLAYER
	bool	Affect_Monster(const int& to_client, const char& affect);

	/* Vergos 관련 함수 */
	void	Hurt_MonsterbyAI(const int& p_id, const int& damage);					// ATTACKED BY AI
	void	Hurt_Vergos(const int& p_id, const int& damage, const char& affect);
	void	Reset_Vergos();

	/* MONSTER STATUS CHANGE */
	void	Change_AttackMode();								// STATUS == ATTACK
	void	Change_ChaseMode();									// STATUS == CHASE
	void	Change_ReactionMode();
	void    Change_DeadMode();									// STATUS == DEATH

private:
	/* MONSTER ALL FUNC */
	void	Change_Animation(const float& fTimeDelta);
	void	Move_NormalMonster(const float& fTimeDelta);		// MOVE PROCESS
	
	/* MONSTER INDIVIDUAL FUNC */
	void	Change_Crab_Animation(const float& fTimeDelta);
	void	Chase_Crab(const float& fTimeDelta);				
	void	Attack_Crab(const float& fTimeDelta);
	void	Dead_Crab(const float& fTimeDelta);

	void	Change_Monkey_Animation(const float& fTimeDelta);
	void	Chase_Monkey(const float& fTimeDelta);
	void	Attack_Monkey(const float& fTimeDelta);
	void	Knockback_Monkey(const float& fTimeDelta);
	void	Dead_Monkey(const float& fTimeDelta);

	/*void	Change_Cloder_Animation(const float& fTimeDelta);
	void	Chase_Cloder(const float& fTimeDelta);
	void	Attack_Cloder(const float& fTimeDelta);
	void	Dead_Cloder(const float& fTimeDelta);*/

	void	Change_DrownedSailor_Animation(const float& fTimeDelta);
	void	Chase_DrownedSailor(const float& fTimeDelta);
	void	Attack_DrownedSailor(const float& fTimeDelta);
	void	Rush_DrownedSailor(const float& fTimeDelta);
	void	Knockback_DrownedSailor(const float& fTimeDelta);
	void	Dead_DrownedSailor(const float& fTimeDelta);

	/*void	Change_GiantBeetle_Animation(const float& fTimeDelta);
	void	Chase_GiantBeetle(const float& fTimeDelta);
	void	Attack_GiantBeetle(const float& fTimeDelta);
	void	Rush_GiantBeetle(const float& fTimeDelta);
	void	Dead_GiantBeetle(const float& fTimeDelta);*/

	void	Change_GiantMonkey_Animation(const float& fTimeDelta);
	void	Chase_GiantMonkey(const float& fTimeDelta);
	void	Attack_GiantMonkey(const float& fTimeDelta);
	void	Rush_GiantMonkey(const float& fTimeDelta);
	void	Knockback_GiantMonkey(const float& fTimeDelta);
	void	Dead_GiantMonkey(const float& fTimeDelta);

	/*void	Change_CraftyArachne_Animation(const float& fTimeDelta);
	void	Chase_CraftyArachne(const float& fTimeDelta);
	void	Attack_CraftyArachne(const float& fTimeDelta);
	void	Dead_CraftyArachne(const float& fTimeDelta);*/

	void	Change_Vergos_Animation(const float& fTimeDelta);
	void	Spawn_Vergos(const float& fTimeDelta);
	void	Choose_VergosPattern(const float& fTimeDelta);
	void	VergosPattern_FirstPhase();
	void	VergosPattern_SecondPhase();
	void	VergosPattern_ThirdPhase();
	void	Attack_Vergos(const float& fTimedelta);
	void	Play_Vergos_NextAttack(chrono::seconds t = 3s);
	void	Dead_Vergos(const float& fTimeDelta);

private:
	void	Play_Animation(const float& fTimeDelta);	
	bool	Is_AnimationSetEnd(const float& fTimeDelta);

	float	Calculate_TargetDist(const _vec3& vPos);

public:
	virtual DWORD Release();

	/* SEND PACKET */
	void	send_Monster_enter_packet(const int& to_client);
	void	send_Monster_move_packet(const int& to_client, const int& ani);
	void	send_Monster_NormalAttack(const int& to_client, const int& ani);
	void	send_Monster_RushAttack(const int& to_client, const int& ani);
	void	send_Monster_Stat(const int& to_client, const int& p_id, const int& damage);
	void	send_Monster_Dead(const int& to_client, const int& ani);
	void	send_Monster_animation_packet(const int& to_client, const int& ani);
	void	send_Monster_Knockback(const int& to_client, const int& ani);
	void	send_Monster_Effect(const int& to_client);

public:
	char			m_monNum				= 0;
	int				m_iHp					= 0;
	int				m_iMaxHp				= 0;
	int				m_iExp					= 0;
	int				m_iMinAtt				= 0;
	int				m_iMaxAtt				= 0;
	float			m_fSpd					= 0;
	
	volatile bool	m_bIsRegen				= false;
	volatile bool	m_bIsAttack				= false;
	volatile bool	m_bIsFighting			= false;
	volatile bool	m_bIsReaction			= false;
	bool			m_bIsShortAttack		= true;		// 근거리 공격
	bool			m_bIsRushAttack			= false;

	int				m_iTargetNum			= -1;
	int				m_iChaseDist			= CHASE_RANGE;
	_vec3			m_vOriPos				= _vec3(0.f);
	_vec3			m_vKnockbackPos			= _vec3(0.f);
	ATTACK_DIST		m_eAttackDist			= ATTACK_DIST::DIST_END;

	int				m_arrAttackPattern[VERGOS_PATTERN];
	int				m_iCurPatternNumber		= 0;
	_vec2			m_arrVergosSkillEffect[3];

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
	float			m_fAnimationSpeed = 1.0f;
};

