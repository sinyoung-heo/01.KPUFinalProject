#pragma once
#include "Obj.h"
class CAi :
    public CObj
{
	enum GLADIATOR_PHASE
	{
		GL_PHASE1, GL_PHASE2, GL_PHASE3, GL_END
	};

public:
	CAi();
	virtual ~CAi();

public:
	void	Set_AnimDuration(double arr[]);
	void	Set_NumAnimation(const _uint& num) { m_uiNumAniIndex = num; }
	void	Set_AnimationKey(const _uint& uiAniKey);

	void	Set_Stop_Attack(chrono::seconds t = 3s);
	void	Set_Start_Attack();
	void	Set_Stop_Reaction(chrono::seconds t = 3s);
	void	Set_Start_Reaction();
public:
	void	Ready_AI(const char& chJob, const char& chWeaponType, const char& chStageID, const _vec3& vPos);
	int		Update_AI(const float& fTimeDelta);
	void	Release_AI();

public:
	void	active_AI();
	void	Change_ActiveMode();
	void	Change_AttackMode();								// STATUS == ATTACK
	void	Change_ChaseMode();									// STATUS == CHASE
	void	Change_ReactionMode();

private:
	void	Change_Animation(const float& fTimeDelta);
	void	Change_Gladiator_Animation(const float& fTimeDelta);
	void	Change_Archer_Animation(const float& fTimeDelta);
	void	Change_Priest_Animation(const float& fTimeDelta);

	/*GLADIATOR */
	void	Set_AnimationSpeed_Gladiator();
	void	process_move_gladiator(const float& fTimeDelta);
	void	process_moveStop_gladiator(const float& fTimeDelta);
	void	Choose_GladiatorPattern(const float& fTimeDelta);
	bool	GladiatorPattern_FirstPhase();
	bool	GladiatorPattern_SecondPhase();
	bool	GladiatorPattern_ThirdPhase();
	void	Attack_Gladiator_AI(const float& fTimedelta);
	void	Play_Gladiator_NextAttack(chrono::seconds t = 0s);
	bool	Is_ComboAttack_Gladiator(const float& fTimeDelta);

	/* ARCHER */
	void	Set_AnimationSpeed_Archer();
	void	process_move_archer(const float& fTimeDelta);
	void	process_moveStop_archer(const float& fTimeDelta);
	void	Choose_ArcherPattern(const float& fTimeDelta);
	void	ArcherPattern_FirstPhase();
	void	ArcherPattern_SecondPhase();
	void	Attack_Archer_AI(const float& fTimedelta);
	void	Play_Archer_NextAttack(chrono::seconds t = 2s);
	bool	Is_ComboAttack_Archer(const float& fTimeDelta);

	/* PRIEST */
	void	Set_AnimationSpeed_Priest();
	void	process_move_priest(const float& fTimeDelta);
	void	process_moveStop_priest(const float& fTimeDelta);
	void	Choose_PriestPattern(const float& fTimeDelta);
	void	PriestPattern_FirstPhase();
	void	PriestPattern_SecondPhase();
	void	Attack_Priest_AI(const float& fTimedelta);
	void	Play_Priest_NextAttack(chrono::seconds t = 5s);
	bool	Is_ComboAttack_Priest(const float& fTimeDelta);

	/* ALL CLASS */
	void	process_disconnect_ai();
	void	process_leave_party_ai();

private:
	void	Play_Animation(const float& fTimeDelta);
	bool	Is_AnimationSetEnd(const float& fTimeDelta, const float& fAnimationSpeed = 4'800.0f);

public:
	void	send_AI_enter_packet(const int& to_client);
	void	send_AI_move_packet(const int& to_client);
	void	send_AI_moveStop_packet(const int& to_client);
	void	send_AI_attack_packet(const int& to_client);
	void	send_AIGladiator_attack_packet(const int& to_client, const float& fAngle);
	void	send_AI_attackStop_packet(const int& to_client);
	void	send_leave_party_ai(const int& to_client);

public:
	virtual DWORD				Release();

public:
	volatile bool	m_bIsAttack		= false;
	volatile bool	m_bIsReaction	= false;
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
	float	m_fSpd					= 0.f;
	float	m_fAnimationSpeed		= 0.f;
	float	m_fLookAngle			= 0.f;

	GLADIATOR_PHASE m_eGladiatorPhase = GLADIATOR_PHASE::GL_END;
	LINEAR_INTERPOLATION_DESC<float> m_tMoveSpeedInterpolationDesc;

	int		m_arrAttackPattern[VERGOS_PATTERN];
	int		m_iCurPatternNumber			= 0;

	/* Animation */
	_uint	m_uiNewAniIndex			= 0;
	_uint	m_uiCurAniIndex			= 0;
	_uint	m_uiNumAniIndex			= 0;
	_uint	m_ui3DMax_NumFrame		= 0;
	_uint	m_ui3DMax_CurFrame		= 0;

	float	m_fAnimationTime		= 0.f;
	float	m_fBlendAnimationTime	= 0.f;
	float	m_fBlendingTime			= 0.f;

	double	m_arrDuration[MAX_ANI_AI]	= { 0 };

	_uint	m_uiAnimIdx				= 0;   // Apply Animation Index
};

