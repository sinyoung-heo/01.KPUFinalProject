#include "stdafx.h"
#include "Ai.h"
#include "Player.h"
#include "Monster.h"

CAi::CAi()
	:m_iLevel(0), m_iHp(0), m_iMaxHp(0), m_fSpd(0.f), m_bIsAttack(false), m_bIsReaction(false), m_pMonster(nullptr),
	m_iMp(0), m_iMaxMp(0), m_iMaxAtt(0), m_iMinAtt(0), m_bIsMove(false), m_fAnimationSpeed(0.f), m_fLookAngle(0.f),
	m_bIsAttackStance(false), m_bIsPartyState(false), m_iPartyNumber(-1), m_eGladiatorPhase(GLADIATOR_PHASE::GL_END),
	m_vOriPos(_vec3(0.f))
{
	memset(m_arrAttackPattern, -1, sizeof(int) * VERGOS_PATTERN);
}

CAi::~CAi()
{
}

void CAi::Set_AnimDuration(double arr[])
{
	for (_uint i = 0; i < MAX_ANI_AI; ++i)
	{
		if (m_uiNumAniIndex > i)
			m_arrDuration[i] = arr[i];
	}
}

void CAi::Set_AnimationKey(const _uint& uiAniKey)
{
	if (m_uiNewAniIndex != uiAniKey)
	{
		m_uiNewAniIndex = uiAniKey;
		m_fBlendingTime = 1.0f;
		m_fBlendAnimationTime = m_fAnimationTime;
	}
}

void CAi::Set_Stop_Attack(chrono::seconds t)
{
	/* 공격이 끝나면 추적 모드로 변경 */
	if (m_bIsAttack)
	{
		bool prev_state = m_bIsAttack;

		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, false))
		{
			if (m_type != PC_PRIEST)
				add_timer(m_sNum, OP_MODE_ACTIVE_AI, system_clock::now() + t);
			else
				add_timer(m_sNum, OP_MODE_CHASE_AI, system_clock::now() + t);
		}
	}
}

void CAi::Set_Start_Attack()
{
	if (!m_bIsAttack)
	{
		bool prev_state = m_bIsAttack;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, true);
	}
}

void CAi::Set_Stop_Reaction(chrono::seconds t)
{
	if (m_bIsReaction)
	{
		bool prev_state = m_bIsReaction;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false);		
	}
}

void CAi::Set_Start_Reaction()
{
	if (!m_bIsReaction)
	{
		bool prev_state = m_bIsReaction;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, true);
	}
}

void CAi::Ready_AI(const char& chJob, const char& chWeaponType, const char& chStageID, const _vec3& vPos)
{
    m_sNum += AI_NUM_START;
    int s_num = m_sNum;

    m_bIsConnect        = false;
    m_bIsDead           = false;

	sprintf_s(m_ID, "AI_%03d", m_sNum);
    m_type              = chJob;
    m_chWeaponType      = chWeaponType;
    m_chStageId         = chStageID;
    m_bIsAttackStance   = true;
    m_bIsPartyState     = true;
    m_iPartyNumber      = RAID_PARTY;

    m_iLevel            = INIT_LEV;
    m_iHp               = AI_HP;
    m_iMaxHp            = AI_HP;
    m_iMp               = AI_MP;
    m_iMaxMp            = AI_MP;
    m_iMinAtt           = AI_MINATT;
    m_iMaxAtt           = AI_MAXATT;
    m_fSpd              = INIT_SPEED;

    m_vPos              = vPos;
	m_vOriPos			= vPos;
    m_vDir              = _vec3(0.f, 0.f, 1.f);
    m_vAngle            = _vec3(0.f, 0.f, 0.f);
	m_status			= STATUS::ST_NONACTIVE;

	// 움직일 방향 설정
	m_pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));

	// Move Speed
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1 = Archer::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2 = Archer::MAX_SPEED * Archer::OTHERS_OFFSET;

    CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));
    CObjMgr::GetInstance()->Add_GameObject(L"AI", this, s_num);
    CObjMgr::GetInstance()->Add_PartyMember(RAID_PARTY, &m_iPartyNumber, s_num);
}

void CAi::Release_AI()
{
    process_disconnect_ai();
}

int CAi::Update_AI(const float& fTimeDelta)
{
	if (m_bIsConnect == false) 
		return NO_EVENT;
	if (fTimeDelta > 1.f)
		return NO_EVENT;

	Change_Animation(fTimeDelta);

	/* Calculate Animation frame */
	Set_AnimationKey(m_uiAnimIdx);
	Play_Animation(fTimeDelta * m_fAnimationSpeed);

	return NO_EVENT;
}

void CAi::Hurt_AI()
{
	random_device					rd;
	default_random_engine			dre{ rd() };
	uniform_int_distribution<int>	HP_Percent{ 1, 5 };
	uniform_int_distribution<int>	MP_Percent{ 0, 5 };
	
	m_iHp -= (int)((float)m_iMaxHp * (float)HP_Percent(dre) / PERCENT);
	m_iMp -= (int)((float)m_iMaxMp * (float)MP_Percent(dre) / PERCENT);

	if (m_iHp <= 0) m_iHp = ZERO_HP;
	if (m_iMp <= 0) m_iMp = ZERO_HP;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->m_bIsPartyState == false) continue;

		send_update_party(raid);
	}
}

void CAi::nonActive_AI()
{
	if (m_status != ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_NONACTIVE);
	}
}

void CAi::active_AI()
{
	if (m_status == ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE);
	}
}

void CAi::Change_ActiveMode()
{
	if (m_status != ST_ACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE);
	}
}

void CAi::Change_AttackMode()
{
	if (m_status != ST_ATTACK)
	{
		STATUS prev_state = m_status;
		if (true == atomic_compare_exchange_strong(&m_status, &prev_state, ST_ATTACK))
			Set_Start_Attack();
	}
}

void CAi::Change_ChaseMode()
{
	if (m_status != ST_CHASE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_CHASE);
	}
}

void CAi::Change_ReactionMode()
{
	if (m_status != ST_REACTION)
	{
		STATUS prev_state = m_status;
		if (true == atomic_compare_exchange_strong(&m_status, &prev_state, ST_REACTION))
			Set_Start_Reaction();
	}
}

void CAi::Change_DeadMode()
{
	if (m_status != ST_DEAD)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_DEAD);
	}
}

void CAi::Change_Animation(const float& fTimeDelta)
{
	switch (m_type)
	{
	case PC_GLADIATOR:
	{
		Change_Gladiator_Animation(fTimeDelta);
		Set_AnimationSpeed_Gladiator();
	}
	break;

	case PC_ARCHER:
	{
		Change_Archer_Animation(fTimeDelta);
		Set_AnimationSpeed_Archer();
	}
	break;

	case PC_PRIEST:
	{
		Change_Priest_Animation(fTimeDelta);
		Set_AnimationSpeed_Priest();
	}
	break;
	}
}

void CAi::Change_Gladiator_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		if (m_bIsMove)
		{
			if (false == CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vTempPos))
			{
				m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Archer::OTHERS_OFFSET;
				m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
				m_fSpd = LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);

				m_vDir = m_vTempPos - m_vPos;
				m_vDir.Normalize();

				m_vPos += m_vDir * m_fSpd * fTimeDelta;
			}
			else
			{
				m_bIsMove = false;
				process_moveStop_gladiator(fTimeDelta);
				Change_ChaseMode();
			}
		}
		else
			process_move_gladiator(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iAniIdx = Gladiator::ATTACK_WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		Choose_GladiatorPattern(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_Gladiator_AI(fTimeDelta);
	}
	break;

	case STATUS::ST_REACTION:
	{
		Play_Gladiator_NextAttack();
	}
	break;

	case STATUS::ST_DEAD:
	{
		reset_ai();
	}
	break;
	}
}

void CAi::Change_Archer_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		if (m_bIsMove)
		{
			if (false == CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vTempPos))
			{
				m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Archer::OTHERS_OFFSET;
				m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
				m_fSpd = LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);

				m_vDir = m_vTempPos - m_vPos;
				m_vDir.Normalize();

				m_vPos += m_vDir * m_fSpd * fTimeDelta;
			}	
			else
			{
				m_bIsMove = false;
				process_moveStop_archer(fTimeDelta);
				Change_ChaseMode();
			}
		}
		else
			process_move_archer(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iAniIdx = Archer::ATTACK_WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		Choose_ArcherPattern(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_Archer_AI(fTimeDelta);
	}
	break;

	case STATUS::ST_REACTION:
	{
		Play_Archer_NextAttack();
	}
	break;

	case STATUS::ST_DEAD:
	{
		reset_ai();
	}
	break;
	}
}

void CAi::Change_Priest_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		if (m_bIsMove)
		{
			if (false == CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vTempPos))
			{
				m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Priest::OTHERS_OFFSET;
				m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
				m_fSpd = LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);

				m_vDir = m_vTempPos - m_vPos;
				m_vDir.Normalize();

				m_vPos += m_vDir * m_fSpd * fTimeDelta;
			}
			else
			{
				m_bIsMove = false;
				process_moveStop_priest(fTimeDelta);
				Change_ChaseMode();
			}
		}
		else
			process_move_priest(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iAniIdx = Priest::ATTACK_WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		Choose_PriestPattern(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_Priest_AI(fTimeDelta);
	}
	break;

	case STATUS::ST_REACTION:
	{
		Play_Priest_NextAttack();
	}
	break;

	case STATUS::ST_DEAD:
	{
		reset_ai();
	}
	break;
	}
}

void CAi::Set_AnimationSpeed_Gladiator()
{
	if (m_uiAnimIdx == Gladiator::TUMBLING)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.60f;
	}
	else if (m_uiAnimIdx == Gladiator::COMBO1 || m_uiAnimIdx == Gladiator::COMBO2)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.25f;
	}
	else if (m_uiAnimIdx == Gladiator::STINGER_BLADE)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.45f;
	}
	else if (m_uiAnimIdx == Gladiator::CUTTING_SLASH)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.35f;
	}
	else if (m_uiAnimIdx == Gladiator::JAW_BREAKER)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.45f;
	}
	else if (m_uiAnimIdx == Gladiator::GAIA_CRUSH1 || m_uiAnimIdx == Gladiator::GAIA_CRUSH3)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.4f;
	}
	else
		m_fAnimationSpeed = Monster_Normal::TPS;
}

void CAi::process_move_gladiator(const float& fTimeDelta)
{
	if (m_bIsMove) return;

	// 움직일 방향 설정
	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
	if (pMonster->Get_Dead()) return;

	m_vDir = pMonster->m_vPos - m_vPos;
	m_vDir.Normalize();

	m_vTempPos = m_vPos + m_vDir * 10.f;
	m_vTempPos.z = 355.f;

	m_iAniIdx = Gladiator::ATTACK_RUN;
	m_uiAnimIdx = Gladiator::ATTACK_RUN;
	Set_AnimationKey(m_uiAnimIdx);

	m_bIsMove = true;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_move_packet(raid);
	}
}

void CAi::process_moveStop_gladiator(const float& fTimeDelta)
{
	// 움직일 방향 설정
	m_iAniIdx = Gladiator::ATTACK_WAIT;
	m_uiAnimIdx = Gladiator::ATTACK_WAIT;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_moveStop_packet(raid);
	}
}

void CAi::Choose_GladiatorPattern(const float& fTimeDelta)
{
	m_eGladiatorPhase = GLADIATOR_PHASE::GL_END;

	if (m_iCurPatternNumber > 2)
		m_iCurPatternNumber = 0;

	switch (m_iCurPatternNumber)
	{
	case 0:
		m_eGladiatorPhase = GLADIATOR_PHASE::GL_PHASE1;
		break;
	case 1:
		m_eGladiatorPhase = GLADIATOR_PHASE::GL_PHASE2;
		break;
	case 2:
		m_eGladiatorPhase = GLADIATOR_PHASE::GL_PHASE3;
		break;
	}

	++m_iCurPatternNumber;
	Change_AttackMode();
}

bool CAi::GladiatorPattern_FirstPhase()
{
	switch (m_uiAnimIdx)
	{
	case Gladiator::COMBO1:
	{
		m_uiAnimIdx = Gladiator::COMBO2;
		m_iAniIdx = Gladiator::COMBO2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::COMBO2:
	{
		m_uiAnimIdx = Gladiator::STINGER_BLADE;
		m_iAniIdx = Gladiator::STINGER_BLADE;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::STINGER_BLADE:
	{
		m_uiAnimIdx = Gladiator::JAW_BREAKER;
		m_iAniIdx = Gladiator::JAW_BREAKER;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::JAW_BREAKER:
	{
		m_uiAnimIdx = Gladiator::CUTTING_SLASH;
		m_iAniIdx = Gladiator::CUTTING_SLASH;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::CUTTING_SLASH:
	{
		m_uiAnimIdx = Gladiator::TUMBLING;
		m_iAniIdx = Gladiator::TUMBLING;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::TUMBLING:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH1;
		m_iAniIdx = Gladiator::GAIA_CRUSH1;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::GAIA_CRUSH1:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH2;
		m_iAniIdx = Gladiator::GAIA_CRUSH2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::GAIA_CRUSH2:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH3;
		m_iAniIdx = Gladiator::GAIA_CRUSH3;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;	
	}

	return false;
}

bool CAi::GladiatorPattern_SecondPhase()
{
	switch (m_uiAnimIdx)
	{
	case Gladiator::COMBO1:
	{
		m_uiAnimIdx = Gladiator::COMBO2;
		m_iAniIdx = Gladiator::COMBO2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::COMBO2:
	{
		m_uiAnimIdx = Gladiator::CUTTING_SLASH;
		m_iAniIdx = Gladiator::CUTTING_SLASH;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::CUTTING_SLASH:
	{
		m_uiAnimIdx = Gladiator::TUMBLING;
		m_iAniIdx = Gladiator::TUMBLING;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::TUMBLING:
	{
		m_uiAnimIdx = Gladiator::STINGER_BLADE;
		m_iAniIdx = Gladiator::STINGER_BLADE;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::STINGER_BLADE:
	{
		m_uiAnimIdx = Gladiator::JAW_BREAKER;
		m_iAniIdx = Gladiator::JAW_BREAKER;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::JAW_BREAKER:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH1;
		m_iAniIdx = Gladiator::GAIA_CRUSH1;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::GAIA_CRUSH1:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH2;
		m_iAniIdx = Gladiator::GAIA_CRUSH2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::GAIA_CRUSH2:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH3;
		m_iAniIdx = Gladiator::GAIA_CRUSH3;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;
	}

	return false;
}

bool CAi::GladiatorPattern_ThirdPhase()
{
	switch (m_uiAnimIdx)
	{
	case Gladiator::COMBO1:
	{
		m_uiAnimIdx = Gladiator::COMBO2;
		m_iAniIdx = Gladiator::COMBO2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::COMBO2:
	{
		m_uiAnimIdx = Gladiator::JAW_BREAKER;
		m_iAniIdx = Gladiator::JAW_BREAKER;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::JAW_BREAKER:
	{
		m_uiAnimIdx = Gladiator::TUMBLING;
		m_iAniIdx = Gladiator::TUMBLING;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::TUMBLING:
	{
		m_uiAnimIdx = Gladiator::STINGER_BLADE;
		m_iAniIdx = Gladiator::STINGER_BLADE;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::STINGER_BLADE:
	{
		m_uiAnimIdx = Gladiator::CUTTING_SLASH;
		m_iAniIdx = Gladiator::CUTTING_SLASH;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::CUTTING_SLASH:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH1;
		m_iAniIdx = Gladiator::GAIA_CRUSH1;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::GAIA_CRUSH1:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH2;
		m_iAniIdx = Gladiator::GAIA_CRUSH2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Gladiator::GAIA_CRUSH2:
	{
		m_uiAnimIdx = Gladiator::GAIA_CRUSH3;
		m_iAniIdx = Gladiator::GAIA_CRUSH3;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;
	}

	return false;
}

void CAi::Attack_Gladiator_AI(const float& fTimedelta)
{
	if (!m_bIsAttack) return;

	if (Gladiator::COMBO1 <= m_uiAnimIdx && m_uiAnimIdx <= Gladiator::TUMBLING)
	{
		if (!Is_AnimationSetEnd(fTimedelta, m_fAnimationSpeed))
			return;
		else
		{
			if (Is_ComboAttack_Gladiator(fTimedelta))
			{			
				if (m_uiAnimIdx == Gladiator::TUMBLING)
				{
					_vec3 vTempDir = m_vDir * -1.f;
					vTempDir.Normalize();

					m_vPos += vTempDir * 2.95f;
					m_fLookAngle = 180.f;
				}
				else
					m_fLookAngle = 0.f;
				
				for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
					if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

					send_AIGladiator_attack_packet(raid, m_fLookAngle);
				}
				// 베르고스 HP 감소
				if (m_pMonster)
					m_pMonster->Hurt_MonsterbyAI(m_sNum, m_iMaxAtt);
			}
			else
			{
				m_uiAnimIdx = Gladiator::ATTACK_WAIT;
				m_iAniIdx = Gladiator::ATTACK_WAIT;
				Set_AnimationKey(m_uiAnimIdx);

				for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
					if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

					send_AI_attackStop_packet(raid);
				}

				Set_Stop_Attack(3s);

			}
			return;
		}
	}

	m_uiAnimIdx = Gladiator::COMBO1;
	m_iAniIdx = Gladiator::COMBO1;
	Set_AnimationKey(m_uiAnimIdx);

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_attack_packet(raid);
	}

	// 베르고스 HP 감소
	if (m_pMonster)
		m_pMonster->Hurt_MonsterbyAI(m_sNum, m_iMaxAtt);
}

void CAi::Play_Gladiator_NextAttack(chrono::seconds t)
{
	if (m_bIsReaction)
	{
		bool prev_state = m_bIsReaction;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false))
		{
			add_timer(m_sNum, OP_MODE_AI_NEXT_ATTACK, system_clock::now() + t);
		}
	}
}

bool CAi::Is_ComboAttack_Gladiator(const float& fTimeDelta)
{
	switch (m_eGladiatorPhase)
	{
	case GLADIATOR_PHASE::GL_PHASE1:
	{		
		return GladiatorPattern_FirstPhase();
	}
	break;

	case GLADIATOR_PHASE::GL_PHASE2:
	{
		return GladiatorPattern_SecondPhase();
	}
	break;

	case GLADIATOR_PHASE::GL_PHASE3:
	{
		return GladiatorPattern_ThirdPhase();
	}
	break;
	}

	return false;
}

void CAi::Set_AnimationSpeed_Archer()
{
	if (m_uiAnimIdx == Archer::ATTACK_ARROW)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.6f;
	}
	else if (m_uiAnimIdx == Archer::ATTACK_RUN)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.35f;
	}
	else if (m_uiAnimIdx == Archer::BACK_DASH)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.25f;
	}
	else if (m_uiAnimIdx == Archer::RAPID_SHOT1 ||
		m_uiAnimIdx == Archer::RAPID_SHOT2)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 2.0f;
	}
	else if (m_uiAnimIdx == Archer::ESCAPING_BOMB)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.25f;
	}
	else if (m_uiAnimIdx == Archer::ARROW_SHOWER_START ||
		m_uiAnimIdx == Archer::ARROW_SHOWER_LOOP ||
		m_uiAnimIdx == Archer::ARROW_SHOWER_SHOT)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 1.65f;
	}
	else if (m_uiAnimIdx == Archer::ARROW_FALL_START ||
		m_uiAnimIdx == Archer::ARROW_FALL_LOOP ||
		m_uiAnimIdx == Archer::ARROW_FALL_SHOT)
	{
		m_fAnimationSpeed = Monster_Normal::TPS * 2.25f;
	}
	else
		m_fAnimationSpeed = Monster_Normal::TPS;
}

void CAi::process_move_archer(const float& fTimeDelta)
{
	if (m_bIsMove) return;

	// 움직일 방향 설정
	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
	if (pMonster->Get_Dead()) return;

	m_vDir = pMonster->m_vPos - m_vPos;
	m_vDir.Normalize();

	m_vTempPos = m_vPos + m_vDir * 10.f;
	m_vTempPos.z = 350.f;

	m_iAniIdx = Archer::ATTACK_RUN;
	m_uiAnimIdx = Archer::ATTACK_RUN;
	Set_AnimationKey(m_uiAnimIdx);

	m_bIsMove = true;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_move_packet(raid);
	}
}

void CAi::process_moveStop_archer(const float& fTimeDelta)
{
	// 움직일 방향 설정
	m_iAniIdx = Archer::ATTACK_WAIT;
	m_uiAnimIdx = Archer::ATTACK_WAIT;
	
	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_moveStop_packet(raid);
	}
}

void CAi::Choose_ArcherPattern(const float& fTimeDelta)
{
	if (rand() % 5 == 0)
		ArcherPattern_FirstPhase();
	else  
		ArcherPattern_SecondPhase();

	m_iCurPatternNumber = 0;
	Change_AttackMode();
}

void CAi::ArcherPattern_FirstPhase()
{
	m_arrAttackPattern[0] = Archer::ATTACK_ARROW;
	m_arrAttackPattern[1] = Archer::RAPID_SHOT1;
	m_arrAttackPattern[2] = Archer::ARROW_FALL_START;
	m_arrAttackPattern[3] = Archer::BACK_DASH;
	m_arrAttackPattern[4] = Archer::ARROW_SHOWER_START;
	m_arrAttackPattern[5] = Archer::CHARGE_ARROW_START;
	m_arrAttackPattern[6] = Archer::BACK_DASH;
}

void CAi::ArcherPattern_SecondPhase()
{
	m_arrAttackPattern[0] = Archer::ATTACK_ARROW;
	m_arrAttackPattern[1] = Archer::RAPID_SHOT1;
	m_arrAttackPattern[2] = Archer::ARROW_FALL_START;
	m_arrAttackPattern[3] = Archer::BACK_DASH;
	m_arrAttackPattern[4] = Archer::ARROW_SHOWER_START;
	m_arrAttackPattern[5] = Archer::CHARGE_ARROW_START;
	m_arrAttackPattern[6] = Archer::BACK_DASH;
}

void CAi::Attack_Archer_AI(const float& fTimedelta)
{
	if (!m_bIsAttack) return;

	if (Archer::BACK_DASH <= m_uiAnimIdx && m_uiAnimIdx <= Archer::CHARGE_ARROW_SHOT)
	{
		if (!Is_AnimationSetEnd(fTimedelta, m_fAnimationSpeed))
			return;
		else
		{
			if (Is_ComboAttack_Archer(fTimedelta))
			{
				for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
					if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

					send_AI_attack_packet(raid);
				}
				// 베르고스 HP 감소
				if (m_pMonster)
					m_pMonster->Hurt_MonsterbyAI(m_sNum, m_iMaxAtt);
			}
			else
			{
				++m_iCurPatternNumber;

				m_uiAnimIdx = Archer::ATTACK_WAIT;
				m_iAniIdx = Archer::ATTACK_WAIT;
				Set_AnimationKey(m_uiAnimIdx);

				for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
					if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

					send_AI_attackStop_packet(raid);
				}

				if (m_iCurPatternNumber >= VERGOS_PATTERN)
					Set_Stop_Attack(1s);
				else
					Change_ReactionMode();
				
			}
			return;
		}
	}

	m_uiAnimIdx = m_arrAttackPattern[m_iCurPatternNumber];
	m_iAniIdx = m_arrAttackPattern[m_iCurPatternNumber];
	Set_AnimationKey(m_uiAnimIdx);

	if (m_uiAnimIdx == Archer::BACK_DASH)
	{
		_vec3 vTempDir = m_vDir * -1.f;
		vTempDir.Normalize();

		m_vPos += vTempDir * 2.8f;
	}

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_attack_packet(raid);
	}
	// 베르고스 HP 감소
	if (m_pMonster)
		m_pMonster->Hurt_MonsterbyAI(m_sNum, m_iMaxAtt);
}

bool CAi::Is_ComboAttack_Archer(const float& fTimeDelta)
{
	switch (m_uiAnimIdx)
	{
	case Archer::RAPID_SHOT1:
	{	
		m_uiAnimIdx = Archer::RAPID_SHOT2;
		m_iAniIdx = Archer::RAPID_SHOT2;
		Set_AnimationKey(m_uiAnimIdx);
		return true;		
	}
	break;

	case Archer::ARROW_SHOWER_START:
	{	
		m_uiAnimIdx = Archer::ARROW_SHOWER_LOOP;
		m_iAniIdx = Archer::ARROW_SHOWER_LOOP;
		Set_AnimationKey(m_uiAnimIdx);
		return true;	
	}
	break;

	case Archer::ARROW_SHOWER_LOOP:
	{	
		m_uiAnimIdx = Archer::ARROW_SHOWER_SHOT;
		m_iAniIdx = Archer::ARROW_SHOWER_SHOT;
		Set_AnimationKey(m_uiAnimIdx);
		return true;		
	}
	break;

	case Archer::ARROW_FALL_START:
	{	
		m_uiAnimIdx = Archer::ARROW_FALL_LOOP;
		m_iAniIdx = Archer::ARROW_FALL_LOOP;
		Set_AnimationKey(m_uiAnimIdx);
		return true;	
	}
	break;

	case Archer::ARROW_FALL_LOOP:
	{		
		m_uiAnimIdx = Archer::ARROW_FALL_SHOT;
		m_iAniIdx = Archer::ARROW_FALL_SHOT;
		Set_AnimationKey(m_uiAnimIdx);
		return true;		
	}
	break;

	case Archer::CHARGE_ARROW_START:
	{		
		m_uiAnimIdx = Archer::CHARGE_ARROW_LOOP;
		m_iAniIdx = Archer::CHARGE_ARROW_LOOP;
		Set_AnimationKey(m_uiAnimIdx);
		return true;		
	}
	break;

	case Archer::CHARGE_ARROW_LOOP:
	{		
		m_uiAnimIdx = Archer::CHARGE_ARROW_SHOT;
		m_iAniIdx = Archer::CHARGE_ARROW_SHOT;
		Set_AnimationKey(m_uiAnimIdx);
		return true;		
	}
	break;
	}

	return false;
}

void CAi::Play_Archer_NextAttack(chrono::seconds t)
{
	if (m_bIsReaction)
	{
		chrono::seconds nextTime = 0s;
		if (m_arrAttackPattern[m_iCurPatternNumber] == Archer::BACK_DASH)
			nextTime = 1s;
		else
			nextTime = 2s;

		bool prev_state = m_bIsReaction;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false))
		{
			add_timer(m_sNum, OP_MODE_AI_NEXT_ATTACK, system_clock::now() + nextTime);
		}
	}
}

void CAi::Set_AnimationSpeed_Priest()
{
	if (m_uiAnimIdx == Priest::ATTACK_WAIT)	
		m_fAnimationSpeed = Monster_Normal::TPS * 0.8f;	
	else if (m_uiAnimIdx == Priest::HASTE)
		m_fAnimationSpeed = Monster_Normal::TPS * 1.35f;	
	else
		m_fAnimationSpeed = Monster_Normal::TPS;
}

void CAi::process_move_priest(const float& fTimeDelta)
{
	if (m_bIsMove) return;

	// 움직일 방향 설정
	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", g_iVergosServerNum));
	if (pMonster->Get_Dead()) return;

	m_vDir = pMonster->m_vPos - m_vPos;
	m_vDir.Normalize();

	m_vTempPos = m_vPos + m_vDir * 10.f;
	m_vTempPos.z = 345.f;

	m_iAniIdx = Priest::ATTACK_RUN;
	m_uiAnimIdx = Priest::ATTACK_RUN;
	Set_AnimationKey(m_uiAnimIdx);

	m_bIsMove = true;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_move_packet(raid);
	}
}

void CAi::process_moveStop_priest(const float& fTimeDelta)
{
	// 움직일 방향 설정
	m_iAniIdx = Priest::ATTACK_WAIT;
	m_uiAnimIdx = Priest::ATTACK_WAIT;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_moveStop_packet(raid);
	}
}

void CAi::Choose_PriestPattern(const float& fTimeDelta)
{
	if (rand() % 5 == 0)
		PriestPattern_FirstPhase();
	else
		PriestPattern_SecondPhase();

	m_iCurPatternNumber = 0;
	Change_AttackMode();
}

void CAi::PriestPattern_FirstPhase()
{
	m_arrAttackPattern[0] =Priest::HASTE;
	m_arrAttackPattern[1] =Priest::AURA_ON;
	m_arrAttackPattern[2] =Priest::HASTE;
	m_arrAttackPattern[3] =Priest::PURIFY;
	m_arrAttackPattern[4] =Priest::HEAL_START;
	m_arrAttackPattern[5] =Priest::HASTE;
	m_arrAttackPattern[6] =Priest::MP_CHARGE_START;
}

void CAi::PriestPattern_SecondPhase()
{
	m_arrAttackPattern[0] = Priest::HASTE;
	m_arrAttackPattern[1] = Priest::AURA_ON;
	m_arrAttackPattern[2] = Priest::HASTE;
	m_arrAttackPattern[3] = Priest::PURIFY;
	m_arrAttackPattern[4] = Priest::HEAL_START;
	m_arrAttackPattern[5] = Priest::HASTE;
	m_arrAttackPattern[6] = Priest::MP_CHARGE_START;
}

void CAi::Attack_Priest_AI(const float& fTimedelta)
{
	if (!m_bIsAttack) return;

	if (Priest::HASTE <= m_uiAnimIdx && m_uiAnimIdx <= Priest::MP_CHARGE_END)
	{
		if (!Is_AnimationSetEnd(fTimedelta, m_fAnimationSpeed))
			return;
		else
		{
			if (Is_ComboAttack_Priest(fTimedelta))
			{
				for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
					if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

					send_AI_attack_packet(raid);
				}
			}
			else
			{
				++m_iCurPatternNumber;

				m_uiAnimIdx = Priest::ATTACK_WAIT;
				m_iAniIdx = Priest::ATTACK_WAIT;
				Set_AnimationKey(m_uiAnimIdx);

				for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
					if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

					send_AI_attackStop_packet(raid);
				}

				if (m_iCurPatternNumber >= VERGOS_PATTERN)
					Set_Stop_Attack(1s);
				else
					Change_ReactionMode();

			}
			return;
		}
	}

	m_uiAnimIdx = m_arrAttackPattern[m_iCurPatternNumber];
	m_iAniIdx = m_arrAttackPattern[m_iCurPatternNumber];
	Set_AnimationKey(m_uiAnimIdx);

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_AI_attack_packet(raid);
	}
}

void CAi::Play_Priest_NextAttack(chrono::seconds t)
{
	if (m_bIsReaction)
	{
		chrono::seconds nextTime = 0s;
		if (m_arrAttackPattern[m_iCurPatternNumber] == Priest::HASTE)
			nextTime = 3s;
		else
			nextTime = 5s;

		bool prev_state = m_bIsReaction;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false))
		{
			add_timer(m_sNum, OP_MODE_AI_NEXT_ATTACK, system_clock::now() + nextTime);
		}
	}
}

bool CAi::Is_ComboAttack_Priest(const float& fTimeDelta)
{
	switch (m_uiAnimIdx)
	{
	case Priest::HEAL_START:
	{
		m_uiAnimIdx = Priest::HEAL_LOOP;
		m_iAniIdx = Priest::HEAL_LOOP;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Priest::HEAL_LOOP:
	{
		m_uiAnimIdx = Priest::HEAL_SHOT;
		m_iAniIdx = Priest::HEAL_SHOT;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Priest::MP_CHARGE_START:
	{
		m_uiAnimIdx = Priest::MP_CHARGE_LOOP;
		m_iAniIdx = Priest::MP_CHARGE_LOOP;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;

	case Priest::MP_CHARGE_LOOP:
	{
		m_uiAnimIdx = Priest::MP_CHARGE_END;
		m_iAniIdx = Priest::MP_CHARGE_END;
		Set_AnimationKey(m_uiAnimIdx);
		return true;
	}
	break;	
	}

	return false;
}

void CAi::reset_ai()
{
	m_bIsConnect		= false;
	m_bIsAttackStance	= true;
	m_bIsPartyState		= true;

	m_bIsMove			= false;
	m_bIsAttack			= false;
	m_bIsReaction		= false;
	
	m_iCurPatternNumber = 0;

	m_vPos				= m_vOriPos;
	m_fLookAngle		= 0.f;
	m_iHp				= m_iMaxHp;
	m_iMp				= m_iMaxMp;

	m_uiAnimIdx			= Gladiator::ATTACK_WAIT;
	m_iAniIdx			= Gladiator::ATTACK_WAIT;

	nonActive_AI();
}

void CAi::process_disconnect_ai()
{
	/* sector에서 해당 플레이어 지우기 */
	CSectorMgr::GetInstance()->Leave_ClientInSector(m_sNum, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));

	/* 파티에 가입되어 있다면 파티 탈퇴 */
	if (m_bIsPartyState == true)
		process_leave_party_ai();

	m_bIsConnect	= false;
	m_bIsDead		= false;
	m_vPos			= _vec3(0.f, 0.f, 0.f);
	m_vDir			= _vec3(0.f, 0.f, 0.f);
	m_ID[0]			= 0;
	m_type			= 0;
	m_chStageId		= STAGE_WINTER;
	m_bIsPartyState = true;
	m_iPartyNumber	= INIT_PARTY_NUMBER;
	m_status		= STATUS::ST_END;

	CObjPoolMgr::GetInstance()->return_Object(L"AI", this);
	CObjMgr::GetInstance()->Delete_GameObject(L"AI", this);
}

void CAi::process_leave_party_ai()
{
	if (!m_bIsPartyState) return;

	// 파티 구성원들에게 파티 탈퇴를 알림
	for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(m_iPartyNumber))
	{
		if (p != m_sNum && CObjMgr::GetInstance()->Is_Player(p))
		{
			// 탈퇴 멤버 정보 -> 기존 구성원
			send_leave_party_ai(p);
		}
	}

	// 해당 유저의 파티 정보 초기화
	CObjMgr::GetInstance()->Leave_Party(&m_iPartyNumber, m_sNum);
	m_bIsPartyState = false;
}

void CAi::Play_Animation(const float& fTimeDelta)
{
	if (m_uiCurAniIndex >= m_uiNumAniIndex) return;

	/*__________________________________________________________________________________________________________
	[ 애니메이션이 계속 반복되도록 fmod 수행 ]
	____________________________________________________________________________________________________________*/
	if (m_uiNewAniIndex != m_uiCurAniIndex)
	{
		m_fAnimationTime = m_fBlendAnimationTime;
		m_fBlendingTime -= 0.001f * fTimeDelta;

		if (m_fBlendingTime <= 0.0f)
			m_fBlendingTime = 0.0f;
	}
	else
	{
		m_fAnimationTime += fTimeDelta;
	}

	m_fAnimationTime = (float)(fmod(m_fAnimationTime, (m_arrDuration[m_uiCurAniIndex])));

	/*__________________________________________________________________________________________________________
	[ 3DMax 상에서의 Frame 계산 ]
	____________________________________________________________________________________________________________*/
	m_ui3DMax_NumFrame = (_uint)(_3DMAX_FPS * (m_arrDuration[m_uiCurAniIndex] / Monster_Normal::TPS));
	m_ui3DMax_CurFrame = (_uint)(_3DMAX_FPS * (m_fAnimationTime / Monster_Normal::TPS));

	if (m_fBlendingTime <= 0.0f)
	{
		m_uiCurAniIndex = m_uiNewAniIndex;
		m_fAnimationTime = 0.0f;
		m_fBlendingTime = 1.f;
	}
}

bool CAi::Is_AnimationSetEnd(const float& fTimeDelta, const float& fAnimationSpeed)
{
	if ((m_fAnimationTime >= m_arrDuration[m_uiCurAniIndex] -
		static_cast<double>(fAnimationSpeed * ANIMA_INTERPOLATION * fTimeDelta)) &&
		(m_uiCurAniIndex == m_uiNewAniIndex))
	{
		return true;
	}

	return false;
}

void CAi::send_AI_enter_packet(const int& to_client)
{
	sc_packet_enter p;

	p.size				= sizeof(p);
	p.type				= SC_PACKET_ENTER;
	p.id				= m_sNum;

	c_lock.lock();
	strncpy_s(p.name, m_ID, strlen(m_ID));
	c_lock.unlock();

	p.o_type			= m_type;
	p.weaponType		= m_chWeaponType;
	p.stageID			= m_chStageId;
	p.is_stance_attack	= m_bIsAttackStance;
	p.is_party_state	= m_bIsPartyState;

	p.posX				= m_vPos.x;
	p.posY				= m_vPos.y;
	p.posZ				= m_vPos.z;

	p.dirX				= m_vDir.x;
	p.dirY				= m_vDir.y;
	p.dirZ				= m_vDir.z;
	
	send_packet(to_client, &p);
}

void CAi::send_AI_move_packet(const int& to_client)
{
	sc_packet_move p;

	p.size		= sizeof(p);
	p.type		= SC_PACKET_MOVE;
	p.id		= m_sNum;

	p.spd		= m_fSpd;
	p.animIdx	= m_iAniIdx;
	p.move_time = move_time;

	p.posX		= m_vTempPos.x;
	p.posY		= m_vTempPos.y;
	p.posZ		= m_vTempPos.z;

	p.dirX		= m_vDir.x;
	p.dirY		= m_vDir.y;
	p.dirZ		= m_vDir.z;

	send_packet(to_client, &p);
}

void CAi::send_AI_moveStop_packet(const int& to_client)
{
	sc_packet_move p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE_STOP;
	p.id = m_sNum;

	p.spd = m_fSpd;
	p.animIdx = m_iAniIdx;
	p.move_time = move_time;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	send_packet(to_client, &p);
}

void CAi::send_AI_attack_packet(const int& to_client)
{
	sc_packet_attack p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ATTACK;
	p.id = m_sNum;

	p.o_type = m_type;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	p.animIdx = m_iAniIdx;
	p.end_angleY = -1.f;

	send_packet(to_client, &p);
}

void CAi::send_AIGladiator_attack_packet(const int& to_client, const float& fAngle)
{
	sc_packet_attack p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ATTACK;
	p.id = m_sNum;

	p.o_type = m_type;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	p.animIdx = m_iAniIdx;
	p.end_angleY = fAngle;

	send_packet(to_client, &p);
}

void CAi::send_AI_attackStop_packet(const int& to_client)
{
	sc_packet_attack p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ATTACK_STOP;
	p.id = m_sNum;

	p.o_type = m_type;
	p.animIdx = m_iAniIdx;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	send_packet(to_client, &p);
}

void CAi::send_enter_party(const int& to_client)
{
	sc_packet_enter_party p;

	p.size		= sizeof(p);
	p.type		= SC_PACKET_ENTER_PARTY;
	p.id		= m_sNum;

	strncpy_s(p.name, m_ID, strlen(m_ID));
	p.o_type	= m_type;
	p.hp		= m_iHp;
	p.maxHp		= m_iMaxHp;
	p.mp		= m_iMp;
	p.maxMp		= m_iMaxMp;

	send_packet(to_client, &p);
}

void CAi::send_leave_party_ai(const int& to_client)
{
	sc_packet_suggest_party p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_LEAVE_PARTY;
	p.id	= m_sNum;

	send_packet(to_client, &p);
}

void CAi::send_update_party(const int& to_client)
{
	sc_packet_update_party p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_UPDATE_PARTY;
	p.id	= m_sNum;

	p.hp	= m_iHp;
	p.maxHp = m_iMaxHp;
	p.mp	= m_iMp;
	p.maxMp = m_iMaxMp;

	send_packet(to_client, &p);
}

DWORD CAi::Release()
{
    return 0;
}
