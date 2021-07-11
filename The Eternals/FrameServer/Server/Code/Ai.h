#pragma once
#include "Obj.h"
class CAi :
    public CObj
{
public:
	CAi();
	virtual ~CAi();

public:
	void	Set_AnimDuration(double arr[]);
	void	Set_NumAnimation(const _uint& num) { m_uiNumAniIndex = num; }
	void	Set_AnimationKey(const _uint& uiAniKey);

public:
	void	Ready_AI(const char& chJob, const char& chWeaponType, const char& chStageID, const _vec3& vPos);
	int		Update_AI(const float& fTimeDelta);
	void	Release_AI();

public:
	void	active_AI();
	void	Change_AttackMode();								// STATUS == ATTACK
	void	Change_ChaseMode();									// STATUS == CHASE
	void	Change_ReactionMode();

private:
	void	Change_Animation(const float& fTimeDelta);
	void	Change_Gladiator_Animation(const float& fTimeDelta);
	void	Change_Archer_Animation(const float& fTimeDelta);
	void	Change_Priest_Animation(const float& fTimeDelta);

	/* ARCHER */
	void	process_move_archer(const float& fTimeDelta);
	void	process_moveStop_archer(const float& fTimeDelta);
	void	Choose_ArcherPattern(const float& fTimeDelta);
	void	ArcherPattern_FirstPhase();
	void	ArcherPattern_SecondPhase();

	void	process_disconnect_ai();
	void	process_leave_party_ai();

private:
	void	Play_Animation(const float& fTimeDelta);
	bool	Is_AnimationSetEnd(const float& fTimeDelta);

public:
	void	send_AI_enter_packet(const int& to_client);
	void	send_AI_move_packet(const int& to_client);
	void	send_AI_moveStop_packet(const int& to_client);
	void	send_leave_party_ai(const int& to_client);

public:
	virtual DWORD				Release();

public:
	bool	m_bIsMove				= false;
	bool	m_bIsAttackStance		= false;
	bool	m_bIsPartyState			= false;
	char	m_chWeaponType			= 0;
	int		m_iPartyNumber			= -1;
	int		m_iAniIdx				= 0;
	int		m_iLevel				= 0;
	int		m_iHp					= 0; 
	int		m_iMaxHp				= 0;
	int		m_iMp					= 0;
	int		m_iMaxMp				= 0;	
	int		m_iMinAtt				= 0;
	int		m_iMaxAtt				= 0;
	float	m_fSpd					= 0;

	LINEAR_INTERPOLATION_DESC<float> m_tMoveSpeedInterpolationDesc;

	int		m_arrAttackPattern[VERGOS_PATTERN];
	int		m_iCurPatternNumber		= 0;

	/* Animation */
	_uint	m_uiNewAniIndex			= 0;
	_uint	m_uiCurAniIndex			= 0;
	_uint	m_uiNumAniIndex			= 0;
	_uint	m_ui3DMax_NumFrame		= 0;
	_uint	m_ui3DMax_CurFrame		= 0;

	float	m_fAnimationTime		= 0.f;
	float	m_fBlendAnimationTime	= 0.f;
	float	m_fBlendingTime			= 0.f;

	double	m_arrDuration[MAX_ANI]	= { 0 };

	_uint	m_uiAnimIdx				= 0;   // Apply Animation Index
};

