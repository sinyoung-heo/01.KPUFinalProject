#include "stdafx.h"
#include "Monster.h"
#include "Player.h"
#include "Ai.h"

CMonster::CMonster()
	:m_iHp(0), m_iMaxHp(0), m_iExp(0), m_iMinAtt(0), m_iMaxAtt(0), m_fSpd(0.f), m_iChaseDist(CHASE_RANGE),
	m_iTargetNum(-1), m_bIsAttack(false), m_bIsShortAttack(true), m_bIsRegen(false),
	m_bIsRushAttack(false), m_bIsFighting(false), m_monNum(0), m_uiAnimIdx(0), m_bIsReaction(false),
	m_vKnockbackPos(_vec3(0.f)), m_eAttackDist(ATTACK_DIST::DIST_END), m_iCurPatternNumber(0)
{
	memset(m_arrAttackPattern, -1, sizeof(int) * VERGOS_PATTERN);
	memset(m_arrVergosSkillEffect, 0, sizeof(_vec2) * 3);
}

CMonster::~CMonster()
{
}

void CMonster::Set_AnimDuration(double arr[])
{
	for (_uint i = 0; i < MAX_ANI; ++i)
	{
		if (m_uiNumAniIndex > i)
			m_arrDuration[i] = arr[i];
	}
}

void CMonster::Ready_Monster(const _vec3& pos, const _vec3& angle, const char& type, const char& num, const char& naviType, const int& hp, const int& att, const int& exp, const float& spd)
{
	/* NPC의 정보 초기화 */
	m_sNum += MON_NUM_START;
	int s_num = m_sNum;

	m_chStageId = naviType;

	if (num == MON_VERGOS)
		m_bIsConnect = false;
	else
		m_bIsConnect = true;

	m_bIsDead = false;

	m_vPos		= pos;
	m_vTempPos	= m_vPos;
	m_vOriPos	= m_vPos;
	m_vDir		= _vec3(0.f, 0.f, 1.f);
	m_vAngle	= angle;
	m_iHp		= hp;
	m_iMaxHp	= hp;
	m_iMinAtt	= att / 2;
	m_iMaxAtt	= att;
	m_iExp		= exp;
	m_fSpd		= spd;
	m_type		= type;
	m_monNum	= num;
	m_status	= STATUS::ST_NONACTIVE;

	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));
	CObjMgr::GetInstance()->Add_GameObject(L"MONSTER", this, s_num);
}

int CMonster::Update_Monster(const float& fTimeDelta)
{
	if (m_bIsConnect == false) return NO_EVENT;
	if (m_bIsRegen) return NO_EVENT;
	if (fTimeDelta > 1.f) return NO_EVENT;

	Change_Animation(fTimeDelta);

	/* Calculate Animation frame */
	Set_AnimationKey(m_uiAnimIdx);

	if (m_monNum == MON_VERGOS)
	{
		if (m_uiAnimIdx == Vergos::BREATH_FIRE)
			m_fAnimationSpeed = 0.75f;
	}
	else
		m_fAnimationSpeed = 1.0f;

	Play_Animation(fTimeDelta * Monster_Normal::TPS * m_fAnimationSpeed);

	return NO_EVENT;
}

void CMonster::Change_Animation(const float& fTimeDelta)
{
	if (m_monNum == MON_CRAB)
		Change_Crab_Animation(fTimeDelta);
	else if (m_monNum == MON_MONKEY)
		Change_Monkey_Animation(fTimeDelta);
	else if (m_monNum == MON_CLODER)
		Change_Cloder_Animation(fTimeDelta);
	else if (m_monNum == MON_SAILOR)
		Change_DrownedSailor_Animation(fTimeDelta);
	else if (m_monNum == MON_GBEETLE)
		Change_GiantBeetle_Animation(fTimeDelta);
	else if (m_monNum == MON_GMONKEY)
		Change_GiantMonkey_Animation(fTimeDelta);
	else if (m_monNum == MON_ARACHNE)
		Change_CraftyArachne_Animation(fTimeDelta);
	else if (m_monNum == MON_VERGOS)
		Change_Vergos_Animation(fTimeDelta);
}

void CMonster::Change_Crab_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{
		
	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::RUN;
		Chase_Crab(fTimeDelta);
	}
	break;
	
	case STATUS::ST_ATTACK:
	{
		Attack_Crab(fTimeDelta);
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_Crab(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_Monkey_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::RUN;
		Chase_Monkey(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_Monkey(fTimeDelta);
	}
	break;

	case STATUS::ST_REACTION:
	{
		Knockback_Monkey(fTimeDelta);
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_Monkey(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_Cloder_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::RUN;
		Chase_Cloder(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_Cloder(fTimeDelta);
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_Cloder(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_DrownedSailor_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::RUN;
		Chase_DrownedSailor(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_DrownedSailor(fTimeDelta);

		if (m_bIsRushAttack)
		{
			if (SAILOR_RUSH_TICK_START <= m_ui3DMax_CurFrame && m_ui3DMax_CurFrame <= SAILOR_RUSH_TICK_END)
			{
				m_vPos += m_vDir * 2.f * fTimeDelta;
			}
			Rush_DrownedSailor(fTimeDelta);
		}
	}
	break;

	case STATUS::ST_REACTION:
	{
		Knockback_DrownedSailor(fTimeDelta);
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_DrownedSailor(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_GiantBeetle_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Chase_GiantBeetle(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_GiantBeetle(fTimeDelta);

		if (m_bIsRushAttack)
		{
			if (30 <= m_ui3DMax_CurFrame && m_ui3DMax_CurFrame <= 45)
			{
				m_vPos += m_vDir * 15.f * fTimeDelta;
			}
			Rush_GiantBeetle(fTimeDelta);
		}
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_GiantBeetle(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_GiantMonkey_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Chase_GiantMonkey(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_GiantMonkey(fTimeDelta);

		if (m_bIsRushAttack)
		{
			if (m_uiAnimIdx == GiantMonkey::ATTACK_COMBO)
			{
				if (65 <= m_ui3DMax_CurFrame && m_ui3DMax_CurFrame <= 125)
				{
					m_vPos += m_vDir * 5.f * fTimeDelta;
				}
			}
			else if (m_uiAnimIdx <= GiantMonkey::ATTACK_JUMPING)
			{
				if (49 <= m_ui3DMax_CurFrame && m_ui3DMax_CurFrame <= 63)
				{
					m_vPos += m_vDir * 10.f * fTimeDelta;
				}
			}
		
			Rush_GiantMonkey(fTimeDelta);
		}
	}
	break;

	case STATUS::ST_REACTION:
	{
		Knockback_GiantMonkey(fTimeDelta);
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_GiantMonkey(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_CraftyArachne_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Move_NormalMonster(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = Monster_Normal::WALK;
		Chase_CraftyArachne(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_CraftyArachne(fTimeDelta);
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_CraftyArachne(fTimeDelta);
	}
	break;
	}
}

void CMonster::Change_Vergos_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		Spawn_Vergos(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_iTargetNum = -1;
		m_uiAnimIdx = Monster_Normal::WAIT;
	}
	break;

	case STATUS::ST_CHASE:
	{
		// 공격패턴 준비
		Choose_VergosPattern(fTimeDelta);
	}
	break;

	case STATUS::ST_ATTACK:
	{
		Attack_Vergos(fTimeDelta);
	}
	break;

	case STATUS::ST_REACTION:
	{
		Play_Vergos_NextAttack();
	}
	break;

	case STATUS::ST_DEAD:
	{
		Dead_Vergos(fTimeDelta);
	}
	break;
	}
}

void CMonster::Spawn_Vergos(const float& fTimeDelta)
{
	if (m_uiAnimIdx == Vergos::SPAWN)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Fight();
			Change_ChaseMode();
			return;
		}
	}

	m_uiAnimIdx = Vergos::SPAWN;

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_Monster_animation_packet(raid, m_uiAnimIdx);
	}
}

void CMonster::Choose_VergosPattern(const float& fTimeDelta)
{
	// 기본 공격: SWING_LFET/RIGHT & BLOW_LEFT/RIGHT/HEAD & BREATH_FIRE
	// 강화 공격: BLOW_ROTATION & FLY

	if (80.f < ((float)m_iHp / m_iMaxHp * PERCENT))
		VergosPattern_FirstPhase();
	else if (50.f < ((float)m_iHp / m_iMaxHp * PERCENT) && (m_iHp / m_iMaxHp * PERCENT) < 80.f)
		VergosPattern_SecondPhase();
	else if (((float)m_iHp / m_iMaxHp * PERCENT) < 50.f)
		VergosPattern_ThirdPhase();
	
	m_iCurPatternNumber = 0;
	Change_AttackMode();
}

void CMonster::VergosPattern_FirstPhase()
{
	// 1 Phase : HP > 80%
    //       => SWING LEFT - BLOW RIGHT - BREATH FIRE - SWING RIGHT - BLOW LEFT - BREATH FIRE - BLOW HEAD
    //      => BREATH FIRE - BLOW LEFT - SWING RIGHT - BLOW HEAD - SWING LEFT - BREATH FIRE - BLOW RIGHT
	int iRand = rand() % 4;
	switch (iRand)
	{
	case 0:
	case 2:
	{
		m_arrAttackPattern[0] = Vergos::SWING_LEFT;
		m_arrAttackPattern[1] = Vergos::BLOW_RIGHT;
		m_arrAttackPattern[2] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[3] = Vergos::SWING_RIGHT;
		m_arrAttackPattern[4] = Vergos::BLOW_LEFT;
		m_arrAttackPattern[5] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[6] = Vergos::BLOW_HEAD;
	}
	break;

	case 1:
	case 3:
	{
		m_arrAttackPattern[0] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[1] = Vergos::BLOW_LEFT;
		m_arrAttackPattern[2] = Vergos::SWING_RIGHT;
		m_arrAttackPattern[3] = Vergos::BLOW_HEAD;
		m_arrAttackPattern[4] = Vergos::SWING_LEFT;
		m_arrAttackPattern[5] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[6] = Vergos::BLOW_RIGHT;
	}
	break;
	}
}

void CMonster::VergosPattern_SecondPhase()
{
	// 2 Phase : HP > 50%
	// 	   => BLOW_ROTATION - BREATH FIRE - BLOW HEAD - SWING LEFT - SWING RIGHT - BREATH FIRE - BLOW LEFT
	// 	   => BLOW_HEAD - BLOW ROTATION - BREATH FIRE - SWING LEFT - BLOW ROTATION - SWING RIGHT - BLOW LEFT
	int iRand = rand() % 4;
	switch (iRand)
	{
	case 0:
	case 2:
	{
		m_arrAttackPattern[0] = Vergos::BLOW_ROTATION;
		m_arrAttackPattern[1] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[2] = Vergos::BLOW_HEAD;
		m_arrAttackPattern[3] = Vergos::SWING_LEFT;
		m_arrAttackPattern[4] = Vergos::SWING_RIGHT;
		m_arrAttackPattern[5] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[6] = Vergos::BLOW_LEFT;
	}
	break;

	case 1:
	case 3:
	{
		m_arrAttackPattern[0] = Vergos::BLOW_HEAD;
		m_arrAttackPattern[1] = Vergos::BLOW_ROTATION;
		m_arrAttackPattern[2] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[3] = Vergos::SWING_LEFT;
		m_arrAttackPattern[4] = Vergos::BLOW_ROTATION;
		m_arrAttackPattern[5] = Vergos::SWING_RIGHT;
		m_arrAttackPattern[6] = Vergos::BLOW_LEFT;
	}
	break;
	}
}

void CMonster::VergosPattern_ThirdPhase()
{
	// 3 Phase : HP > 30%
	//	   => FLY - SWING RIGHT - BREATH FIRE - BLOW ROTATION - SWING LEFT - BLOW HEAD - BREATH FIRE
	//     => BLOW ROTATION - BREATH FIRE - FLY - BLOW HEAD - SWING RIGHT - BLOW LEFT - BREATH FIRE
	int iRand = rand() % 4;
	switch (iRand)
	{
	case 0:
	case 2:
	{
		m_arrAttackPattern[0] = Vergos::FLY_START;
		m_arrAttackPattern[1] = Vergos::SWING_RIGHT;
		m_arrAttackPattern[2] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[3] = Vergos::BLOW_ROTATION;
		m_arrAttackPattern[4] = Vergos::SWING_LEFT;
		m_arrAttackPattern[5] = Vergos::BLOW_HEAD;
		m_arrAttackPattern[6] = Vergos::BREATH_FIRE;
	}
	break;

	case 1:
	case 3:
	{
		m_arrAttackPattern[0] = Vergos::BLOW_ROTATION;
		m_arrAttackPattern[1] = Vergos::BREATH_FIRE;
		m_arrAttackPattern[2] = Vergos::FLY_START;
		m_arrAttackPattern[3] = Vergos::BLOW_HEAD;
		m_arrAttackPattern[4] = Vergos::SWING_RIGHT;
		m_arrAttackPattern[5] = Vergos::BLOW_LEFT;
		m_arrAttackPattern[6] = Vergos::BREATH_FIRE;
	}
	break;
	}
}

void CMonster::Attack_Vergos(const float& fTimedelta)
{
	if (!m_bIsAttack) return;

	if (Vergos::SWING_RIGHT <= m_uiAnimIdx && m_uiAnimIdx <= Vergos::FLY_END)
	{
		if (!Is_AnimationSetEnd(fTimedelta))
			return;
		else
		{
			/* AI HP 감소 */
			process_hurt_ai();

			++m_iCurPatternNumber;
			m_uiAnimIdx = Monster_Normal::WAIT;

			if (m_iCurPatternNumber >= VERGOS_PATTERN)
				Set_Stop_Attack(1s);
			else
				Change_ReactionMode();
			return;
		}
	}

	m_uiAnimIdx = m_arrAttackPattern[m_iCurPatternNumber];

	/* 베르고스 스킬 소환 */
	if (m_uiAnimIdx == Vergos::FLY_START)
	{
		random_device					rd;
		default_random_engine			dre{ rd() };
		uniform_int_distribution<int>	uid_posX{ 340, 425 };
		uniform_int_distribution<int>	uid_posZ{ 325, 375 };

		for (int i = 0; i < 3; ++i)
		{
			m_arrVergosSkillEffect[i].x = (float)uid_posX(dre);
			m_arrVergosSkillEffect[i].y = (float)uid_posZ(dre);
		}
	}

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;

		send_Monster_animation_packet(raid, m_uiAnimIdx);

		if (m_uiAnimIdx == Vergos::FLY_START)
			send_Monster_Effect(raid);
	}
}

void CMonster::Play_Vergos_NextAttack(chrono::seconds t)
{
	if (m_bIsReaction)
	{
		chrono::seconds nextTime = 0s;
		if (m_arrAttackPattern[m_iCurPatternNumber-1] == Vergos::FLY_START)
			nextTime = 10s;
		else
			nextTime = 3s;

		bool prev_state = m_bIsReaction;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false))
		{
			add_timer(m_sNum, OP_MODE_BOSS_NEXT_ATTACK, system_clock::now() + nextTime);
		}
	}
}

void CMonster::Dead_Vergos(const float& fTimeDelta)
{
	if (m_bIsRegen) return;
	
	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Vergos::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			m_bIsConnect		= false;
			m_iCurPatternNumber = 0;

			Set_Start_Regen(20s);
			Init_AllStatus();

			return;
		}
	}

	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
		if (pPlayer == nullptr || pPlayer->Get_IsConnected() == false) continue;
		
		send_Monster_Dead(raid, Vergos::DEATH);

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	m_uiAnimIdx = Vergos::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Reset_Vergos()
{
	m_bIsConnect		= false;
	m_bIsRegen			= false;
	m_bIsDead			= false;
	m_iHp				= m_iMaxHp;
	m_iCurPatternNumber = 0;

	Init_AllStatus();
	nonActive_monster();
}

void CMonster::Move_NormalMonster(const float& fTimeDelta)
{
	if (m_bIsDead || m_bIsRegen) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	if (m_monNum == MON_CRAB)
		m_fSpd = CRAB_SPD;
	else
		m_fSpd = 1.f;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 이동 전: 인접 섹터 순회
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (obj_num == m_sNum) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Monster 움직임 방향 처리 */
	if (CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vTempPos))
	{
		switch (rand() % 8)
		{
		case 0: m_vDir = _vec3(0.f, 0.f, 1.f); break;
		case 1: m_vDir = _vec3(0.f, 0.f, -1.f); break;
		case 2: m_vDir = _vec3(1.f, 0.f, 0.f); break;
		case 3: m_vDir = _vec3(1.f, 0.f, 1.f); break;
		case 4: m_vDir = _vec3(1.f, 0.f, -1.f); break;
		case 5: m_vDir = _vec3(-1.f, 0.f, 0.f); break;
		case 6: m_vDir = _vec3(-1.f, 0.f, 1.f); break;
		case 7: m_vDir = _vec3(-1.f, 0.f, -1.f); break;
		}

		/* 해당 NPC의 미래 위치 좌표 산출 -> 미래 위치좌표는 임시 변수에 저장 */
		m_vTempPos += m_vDir * 5.f;

		if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vTempPos, m_vOriPos))
		{
			m_vTempPos = m_vPos;
			return;
		}
	}

	/* Monster Move */
	m_vPos += m_vDir * m_fSpd * fTimeDelta;

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;	
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
		

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto& pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}

	}

	// Monster 시야 내에 아무도 없다면 NON ACTIVE로 상태 변경
	if (new_viewlist.empty() == true)
		nonActive_monster();
}

void CMonster::Chase_Crab(const float& fTimeDelta)
{
	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = CRAB_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 추적 범위 내에 있는 유저 탐색한다.
					if (CObjMgr::GetInstance()->Is_Monster_Target(this, pPlayer))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		int target_id = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", target_id));
		if (pTarget == nullptr) 		
			return;
		
		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* monster chase move -> arrive at player -> start to attack */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, pTarget->m_vPos))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			Change_AttackMode();
			return;
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * fTimeDelta;
		else
		{
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	
	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto& pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Chase_Monkey(const float& fTimeDelta)
{
	if (m_bIsReaction) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = MONKEY_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 영역을 벗어났다면 추적 중지
					if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vOriPos, CHASE_RANGE))
					{
						m_iTargetNum = -1;
						m_iChaseDist = 1;
						m_fSpd = GOBACK_SPD;
					}
					else
					{
						// 몬스터 추적 범위 내에 있는 유저 탐색한다.
						if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, m_iChaseDist))
							old_targetList.insert(obj_num);
					}
				}
			}
		}
	}

	/* 타겟(추적 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());
	
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* monster chase move -> arrive at player -> start to attack */

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((THROW_RANGE_MONKEY_START * THROW_RANGE_MONKEY_START) < fDist && fDist < (THROW_RANGE_MONKEY_END * THROW_RANGE_MONKEY_END))
		{
			m_bIsShortAttack = false;
			Change_AttackMode();
			return;
		}
		else if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) >= fDist)
		{
			m_bIsShortAttack = true;
			Change_AttackMode();
			return;
		}
		else if (((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist) || (fDist > (THROW_RANGE_MONKEY_END * THROW_RANGE_MONKEY_END)))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		m_iTargetNum = -1;

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			m_iChaseDist = CHASE_RANGE;
			m_fSpd = MONKEY_CHASE_SPD;
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Chase_Cloder(const float& fTimeDelta)
{
	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = CLODER_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 영역을 벗어났다면 추적 중지
					if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vOriPos, CHASE_RANGE))
					{
						m_iTargetNum = -1;
						m_iChaseDist = 1;
						m_fSpd = GOBACK_SPD;
					}
					else
					{
						// 몬스터 추적 범위 내에 있는 유저 탐색한다.
						if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, m_iChaseDist))
							old_targetList.insert(obj_num);
					}
				}
			}
		}
	}

	/* 타겟(추적 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* monster chase move -> arrive at player -> start to attack */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			Change_AttackMode();
			return;
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		m_iTargetNum = -1;

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			m_iChaseDist = CHASE_RANGE;
			m_fSpd = CLODER_CHASE_SPD;
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto& pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Chase_DrownedSailor(const float& fTimeDelta)
{
	if (m_bIsReaction) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = SAILOR_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 영역을 벗어났다면 추적 중지
					if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vOriPos, CHASE_RANGE))
					{
						m_iTargetNum = -1;
						m_iChaseDist = 1;
						m_fSpd = GOBACK_SPD;
					}
					else
					{
						// 몬스터 추적 범위 내에 있는 유저 탐색한다.
						if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, m_iChaseDist))
							old_targetList.insert(obj_num);
					}
				}
			}
		}
	}

	/* 타겟(추적 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist && fDist < (ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER))
		{
			m_bIsShortAttack = false;
			Change_AttackMode();
			return;
		}
		else if ((ATTACK_RANGE_SAILOR * ATTACK_RANGE_SAILOR) >= fDist)
		{
			m_bIsShortAttack = true;
			Change_AttackMode();
			return;
		}
		else if ((ATTACK_RANGE_SAILOR * ATTACK_RANGE_SAILOR) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;

	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		m_iTargetNum = -1;

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			m_iChaseDist = CHASE_RANGE;
			m_fSpd = SAILOR_CHASE_SPD;
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto& pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Chase_GiantBeetle(const float& fTimeDelta)
{
	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = GIANTBETTLE_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 영역을 벗어났다면 추적 중지
					if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vOriPos, CHASE_RANGE))
					{
						m_iTargetNum = -1;
						m_iChaseDist = 1;
						m_fSpd = GOBACK_SPD;
					}
					else
					{
						// 몬스터 추적 범위 내에 있는 유저 탐색한다.
						if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, m_iChaseDist))
							old_targetList.insert(obj_num);
					}
				}
			}
		}
	}

	/* 타겟(추적 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((RUSH_RANGE_GBEETEL_START * RUSH_RANGE_GBEETEL_START) < fDist && fDist < (RUSH_RANGE_GBEETEL_END * RUSH_RANGE_GBEETEL_END))
		{
			m_bIsShortAttack = false;
			Change_AttackMode();
			return;
		}
		else if ((ATTACK_RANGE_GBEETEL * ATTACK_RANGE_GBEETEL) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else if ((ATTACK_RANGE_GBEETEL * ATTACK_RANGE_GBEETEL) >= fDist)
		{
			m_bIsShortAttack = true;
			Change_AttackMode();
			return;
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		m_iTargetNum = -1;

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			m_iChaseDist = CHASE_RANGE;
			m_fSpd = GIANTBETTLE_CHASE_SPD;
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto& pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Chase_GiantMonkey(const float& fTimeDelta)
{
	if (m_bIsReaction) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = GIANTMONKEY_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 영역을 벗어났다면 추적 중지
					if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vOriPos, CHASE_RANGE))
					{
						m_iTargetNum = -1;
						m_iChaseDist = 1;
						m_fSpd = GOBACK_SPD;
					}
					else
					{
						// 몬스터 추적 범위 내에 있는 유저 탐색한다.
						if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, m_iChaseDist))
							old_targetList.insert(obj_num);
					}		
				}
			}
		}
	}

	/* 타겟(추적 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);

		// ATTACK JUMPING
		if ((RUSH_RANGE_GMONKEY_START * RUSH_RANGE_GMONKEY_START) < fDist && fDist < (RUSH_RANGE_GMONKEY_END * RUSH_RANGE_GMONKEY_END))
		{
			m_eAttackDist = ATTACK_DIST::DIST_LONG;
			Change_AttackMode(); 
			return;
		}
		// ATTACK COMBO, FLYSTAMP
		else if ((ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER) < fDist && fDist < (ATTACK_RANGE_GBEETEL * ATTACK_RANGE_GBEETEL))
		{
			m_eAttackDist = ATTACK_DIST::DIST_MIDDLE;
			Change_AttackMode(); 
			return;
		}		
		// ATTACK 1~3
		else if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) >= fDist)
		{
			m_eAttackDist = ATTACK_DIST::DIST_SHORT;
			Change_AttackMode(); 
			return;
		}
		// CHASE
		else if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		m_iTargetNum = -1;

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			m_iChaseDist = CHASE_RANGE;
			m_fSpd = GIANTMONKEY_CHASE_SPD;
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto& pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto& pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Chase_CraftyArachne(const float& fTimeDelta)
{
	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = ARCHNE_CHASE_SPD;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 이동 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 영역을 벗어났다면 추적 중지
					if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vOriPos, CHASE_RANGE))
					{
						m_iTargetNum = -1;
						m_iChaseDist = 1;
						m_fSpd = 8.f;
					}
					else
					{
						// 몬스터 추적 범위 내에 있는 유저 탐색한다.
						if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, m_iChaseDist))
							old_targetList.insert(obj_num);
					}
				}
			}
		}
	}

	/* 타겟(추적 대상)이 존재할 경우 -> 타겟 추적 */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* 타겟을 추적하기 위한 방향으로 설정 */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* monster chase move -> arrive at player -> start to attack */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((ATTACK_RANGE_GBEETEL * ATTACK_RANGE_GBEETEL) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else
		{
			Change_AttackMode();
			return;
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		m_iTargetNum = -1;

		/* monster return home position */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
			m_vPos += m_vDir * fTimeDelta;
		else
		{
			m_iChaseDist = CHASE_RANGE;
			m_fSpd = ARCHNE_CHASE_SPD;
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (m_chStageId == STAGE_VELIKA)
	{
		if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_BEACH)
	{
		if (CNaviMesh_Beach::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}
	else if (m_chStageId == STAGE_WINTER)
	{
		if (CNaviMesh_Winter::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
			return;
	}

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 이동 후: 인접 섹터 순회 -> 유저가 있을 시 new viewlist 내에 등록
	for (auto& s : nearSectors)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto pl : old_viewlist)
	{
		// 이동 후에도 Monster 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* 해당 유저에게 NPC가 움직인 후의 위치를 전송 */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// 이동 후에 Monster 시야 목록 내에 "pl"(server number) 유저가 없는 경우
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* 해당 유저의 시야 목록에서 현재 Monster 삭제 */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CMonster::Attack_Crab(const float& fTimeDelta)
{
	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Monster_Normal::ATTACK == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Stop_Attack();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;
	 
	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// 몬스터 공격 범위 내에 있는 유저 탐색한다.
					if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, ATTACK_RANGE_CRAB))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (!old_targetList.empty())
	{
		/* 공격 애니메이션 설정 */
		m_uiAnimIdx = Monster_Normal::ATTACK;
		Set_AnimationKey(m_uiAnimIdx);

		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto& pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}		
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_Monkey(const float& fTimeDelta)
{
	if (m_bIsReaction) return;

	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Monster_Normal::ATTACK <= m_uiAnimIdx && m_uiAnimIdx <= Monkey::ATTACK_THROW)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Stop_Attack();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((THROW_RANGE_MONKEY_START * THROW_RANGE_MONKEY_START) < fDist && fDist < (THROW_RANGE_MONKEY_END * THROW_RANGE_MONKEY_END))
		{
			m_bIsShortAttack = false;
		}

		/* 공격 애니메이션 설정 */
		if (m_bIsShortAttack)
			m_uiAnimIdx = Monster_Normal::ATTACK;
		else
			m_uiAnimIdx = Monkey::ATTACK_THROW;

		Set_AnimationKey(m_uiAnimIdx);
		
		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto& pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;		
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_Cloder(const float& fTimeDelta)
{
	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Monster_Normal::ATTACK <= m_uiAnimIdx && m_uiAnimIdx <= Cloder::ATTACK_SWING)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Stop_Attack();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 공격 애니메이션 설정 */
		m_uiAnimIdx = rand() % 3 + 3;
		Set_AnimationKey(m_uiAnimIdx);

		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto& pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_DrownedSailor(const float& fTimeDelta)
{
	if (m_bIsReaction) return;

	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (DrownedSailor::ATTACK_SPIN <= m_uiAnimIdx && m_uiAnimIdx <= DrownedSailor::ATTACK_HOOK)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			m_bIsRushAttack = false;
			Set_Stop_Attack(3s);
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist && fDist < (ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER))
		{
			m_bIsShortAttack = false;
		}

		/* 공격 애니메이션 설정 */
		if (m_bIsShortAttack)
		{
			m_bIsRushAttack = false;

			m_uiAnimIdx = rand() % 5 + 3;
			if (m_uiAnimIdx == DrownedSailor::ATTACK_RUSH)
				m_uiAnimIdx = DrownedSailor::ATTACK_HOOK;

			Set_AnimationKey(m_uiAnimIdx);
		}
		else
		{
			m_bIsRushAttack = true;
			m_uiAnimIdx = DrownedSailor::ATTACK_RUSH;

			Set_AnimationKey(m_uiAnimIdx);
		}

		/* 돌진기 사용 시 따로 패킷 전송 */
		if (m_bIsRushAttack) return;

		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto& pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_GiantBeetle(const float& fTimeDelta)
{
	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (GiantBeetle::ATTACK_UP <= m_uiAnimIdx && m_uiAnimIdx <= GiantBeetle::ATTACK_FLYSTAMP)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			m_bIsRushAttack = false;
			Set_Stop_Attack(5s);
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 원거리 & 근거리 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((RUSH_RANGE_GBEETEL_START * RUSH_RANGE_GBEETEL_START) < fDist && fDist < (RUSH_RANGE_GBEETEL_END * RUSH_RANGE_GBEETEL_END))
		{
			m_bIsShortAttack = false;
		}

		/* 공격 애니메이션 설정 */
		if (m_bIsShortAttack)
		{
			m_bIsRushAttack = false;
			m_uiAnimIdx = rand() % 4 + 2;
			if (m_uiAnimIdx == GiantBeetle::ATTACK_RUSH)
				m_uiAnimIdx = GiantBeetle::ATTACK_DOWN;

			Set_AnimationKey(m_uiAnimIdx);
		}
		else
		{
			m_bIsRushAttack = true;
			m_uiAnimIdx = GiantBeetle::ATTACK_RUSH;

			Set_AnimationKey(m_uiAnimIdx);
		}

		/* 돌진기 사용 시 따로 패킷 전송 */
		if (m_bIsRushAttack) return;

		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto& pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_GiantMonkey(const float& fTimeDelta)
{
	if (m_bIsReaction) return;

	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (GiantMonkey::ATTACK_RIGHT <= m_uiAnimIdx && m_uiAnimIdx <= GiantMonkey::ATTACK_COMBO)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			m_bIsRushAttack = false;
			Set_Stop_Attack(7s);
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* 공격 타입 설정 */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);

		// ATTACK JUMPING
		if ((RUSH_RANGE_GMONKEY_START * RUSH_RANGE_GMONKEY_START) < fDist && fDist < (RUSH_RANGE_GMONKEY_END * RUSH_RANGE_GMONKEY_END))
		{
			m_eAttackDist = ATTACK_DIST::DIST_LONG;
			m_uiAnimIdx = GiantMonkey::ATTACK_JUMPING;
			Set_AnimationKey(m_uiAnimIdx);

			m_bIsRushAttack = true;
		}

		// ATTACK COMBO, FLYSTAMP
		else if ((ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER) < fDist && fDist < (ATTACK_RANGE_GBEETEL * ATTACK_RANGE_GBEETEL))
		{
			m_eAttackDist = ATTACK_DIST::DIST_MIDDLE;
			m_uiAnimIdx = rand() % 2 + 7;
			Set_AnimationKey(m_uiAnimIdx);

			if (m_uiAnimIdx == GiantMonkey::ATTACK_FLYSTAMP)
			{
				m_bIsRushAttack = false;
			}
			else if (m_uiAnimIdx == GiantMonkey::ATTACK_COMBO)
			{
				m_bIsRushAttack = true;
			}
		}

		// ATTACK 1~3 & HOOK
		else if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) >= fDist)
		{
			m_eAttackDist = ATTACK_DIST::DIST_SHORT;
			m_uiAnimIdx = rand() % 4 + 2;
			Set_AnimationKey(m_uiAnimIdx);

			m_bIsRushAttack = false;
		}
		
		/* 돌진기 사용 시 따로 패킷 전송 */
		if (m_bIsRushAttack) return;

		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto& pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_CraftyArachne(const float& fTimeDelta)
{
	// 이전 공격 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (CraftyArachne::ATTACK_POUND <= m_uiAnimIdx && m_uiAnimIdx <= CraftyArachne::ATTACK_FLYSTAMP)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Stop_Attack();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 타겟(공격 대상)이 존재할 경우 -> 몬스터 공격 시작 알리기 */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		m_uiAnimIdx = rand() % 4 + 2;
		Set_AnimationKey(m_uiAnimIdx);

		// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
		for (auto pl : old_viewlist)
		{
			/* 유저일 경우 처리 */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;		
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* 타겟(공격 대상)이 존재하지 않을 경우 -> 생성된 위치로 돌아감 */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Rush_DrownedSailor(const float& fTimeDelta)
{
	if (m_uiAnimIdx != DrownedSailor::ATTACK_RUSH) return;
	if (m_bIsRushAttack == false) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	for (auto& pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{		
			//if (m_uiAnimIdx == DrownedSailor::ATTACK_RUSH)
			{
				send_Monster_RushAttack(pl, m_uiAnimIdx);
			}
		}
	}	
}

void CMonster::Rush_GiantBeetle(const float& fTimeDelta)
{
	if (m_uiAnimIdx != GiantBeetle::ATTACK_RUSH) return;
	if (m_bIsRushAttack == false) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	for (auto pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			if (m_uiAnimIdx == GiantBeetle::ATTACK_RUSH)
			{
				send_Monster_RushAttack(pl, m_uiAnimIdx);
			}
		}
	}
}

void CMonster::Rush_GiantMonkey(const float& fTimeDelta)
{
	if (m_uiAnimIdx != GiantMonkey::ATTACK_JUMPING && m_uiAnimIdx != GiantMonkey::ATTACK_COMBO) return;
	if (m_bIsRushAttack == false) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 공격 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	for (auto& pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			//if (m_uiAnimIdx == GiantMonkey::ATTACK_JUMPING || m_uiAnimIdx == GiantMonkey::ATTACK_COMBO)
			{
				send_Monster_RushAttack(pl, m_uiAnimIdx);
			}
		}
	}
}

void CMonster::Knockback_Monkey(const float& fTimeDelta)
{
	if (m_bIsReaction == false) return;
	if (Monkey::DOWN == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;

		else
		{
			Set_Stop_Reaction(3s);
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 넉백 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 넉백 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 넉백 후의 위치 갱신
	m_vKnockbackPos = m_vPos + (-1.f * m_vDir) * NUCKBACK_DIST;

	for (auto& pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			send_Monster_Knockback(pl, Monkey::DOWN);
		}
	}

	m_uiAnimIdx = Monkey::DOWN;
	Set_AnimationKey(Monkey::DOWN);
}

void CMonster::Knockback_DrownedSailor(const float& fTimeDelta)
{
	if (m_bIsReaction == false) return;
	if (DrownedSailor::DOWN == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;

		else
		{
			Set_Stop_Reaction(3s);
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 넉백 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 넉백 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 넉백 후의 위치 갱신
	m_vKnockbackPos = m_vPos + (-1.f * m_vDir) * NUCKBACK_DIST;

	for (auto& pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			send_Monster_Knockback(pl, DrownedSailor::DOWN);
		}
	}

	m_uiAnimIdx = DrownedSailor::DOWN;
	Set_AnimationKey(DrownedSailor::DOWN);
}

void CMonster::Knockback_GiantMonkey(const float& fTimeDelta)
{
	if (m_bIsReaction == false) return;
	if (GiantMonkey::DOWN == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		
		else
		{
			Set_Stop_Reaction(3s);
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 넉백 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// 넉백 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// 넉백 후의 위치 갱신
	m_vKnockbackPos = m_vPos + (-1.f * m_vDir) * NUCKBACK_DIST;

	for (auto& pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{	
			send_Monster_Knockback(pl, GiantMonkey::DOWN);	
		}
	}

	m_uiAnimIdx = GiantMonkey::DOWN;
	Set_AnimationKey(GiantMonkey::DOWN);
}

void CMonster::Dead_Crab(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Crab::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(30s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto& pl : old_viewlist)
	{
		send_Monster_Dead(pl, Crab::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = Crab::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_Monkey(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Monkey::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(50s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto& pl : old_viewlist)
	{
		send_Monster_Dead(pl, Monkey::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = Monkey::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_Cloder(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (Cloder::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(10s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto& pl : old_viewlist)
	{
		send_Monster_Dead(pl, Cloder::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = Cloder::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_DrownedSailor(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (DrownedSailor::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(60s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto& pl : old_viewlist)
	{
		send_Monster_Dead(pl, DrownedSailor::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = DrownedSailor::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_GiantBeetle(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (GiantBeetle::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(10s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, GiantBeetle::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = GiantBeetle::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_GiantMonkey(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (GiantMonkey::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(60s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto& pl : old_viewlist)
	{
		send_Monster_Dead(pl, GiantMonkey::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = GiantMonkey::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_CraftyArachne(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// 애니메이션이 아직 종료되지 않았을 경우 -> 몬스터 공격 패킷 전송 X
	if (CraftyArachne::DEATH == m_uiAnimIdx)
	{
		if (!Is_AnimationSetEnd(fTimeDelta))
			return;
		else
		{
			Set_Start_Regen(60s);
			Init_AllStatus();
			return;
		}
	}

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 죽는 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 공격 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List 내의 유저들에게 해당 Monster의 공격 시작을 알림.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, CraftyArachne::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* 애니메이션 설정 */
	m_uiAnimIdx = CraftyArachne::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Hurt_Monster(const int& p_id, const int& damage, const char& affect)
{
	if (m_bIsRegen || m_bIsDead) return;

	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 피격당하기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 피격당하기 전: 인접 섹터 순회 (몬스터 시야 파악)
	for (auto& s : oldnearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 유저의 서버 번호 추출
			for (auto& obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 유저일 경우 처리 */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pPlayer->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* 피격 당함 */
	if (m_iHp > ZERO_HP)
	{
		m_iHp -= damage;		
	}

	/* Monster Dead */
	else if (m_iHp <= ZERO_HP)
	{
		m_iHp		= ZERO_HP;
		m_bIsDead	= true;

		/* Increase Player Exp */
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", p_id));
		if (pPlayer == nullptr) return;

		pPlayer->m_iExp += m_iExp;

		/* Player Level Up */
		if (pPlayer->m_iExp >= pPlayer->m_iMaxExp)
		{
			pPlayer->m_iLevel	+= INIT_LEV;
			pPlayer->m_iExp		= INIT_EXP;
			pPlayer->m_iMaxExp	+= pPlayer->m_iLevel * INCREASE_EXP;
			pPlayer->m_iMaxHp	+= pPlayer->m_iLevel * INCREASE_HP;
			pPlayer->m_iHp		= pPlayer->m_iMaxHp;
			pPlayer->m_iMaxMp	+= pPlayer->m_iLevel * INCREASE_MP;
			pPlayer->m_iMp		= pPlayer->m_iMaxMp;
			pPlayer->m_iMinAtt	+= pPlayer->m_iLevel * INCREASE_ATT;
			pPlayer->m_iMaxAtt	+= pPlayer->m_iLevel * INCREASE_ATT;
		}
		pPlayer->send_player_stat(p_id);

		Change_DeadMode();
		return;
	}

	// Monster View List 내의 유저들에게 해당 Monster의 변경된 stat을 알림.
	for (auto& pl : old_viewlist)
	{
		/* 유저일 경우 처리 */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			if (m_bIsDead) return;
			send_Monster_Stat(pl, p_id, damage);

			/* Affect */
			if (Affect_Monster(pl, affect) == true)
				return;
		}
	}

	/* Monster Attack Target 설정 */
	if (m_iTargetNum == -1)
	{
		m_iTargetNum = p_id;
		m_iChaseDist = CHASE_RANGE;
	}

	/* 추적 상태로 변경 */
	if (m_bIsFighting == false)
	{
		Set_Start_Fight();
		Change_ChaseMode();
	}
}

bool CMonster::Affect_Monster(const int& to_client, const char& affect)
{
	if (m_monNum != MON_GMONKEY && m_monNum != MON_MONKEY && m_monNum != MON_SAILOR)
		return false;

	if (m_bIsAttack == true || m_bIsReaction == true)
		return false;

	if (m_monNum == MON_GMONKEY)
	{
		if (affect == AFFECT_FINCH)
			send_Monster_animation_packet(to_client, GiantMonkey::FINCH);
		else if (affect == AFFECT_GROGGY)
			send_Monster_animation_packet(to_client, GiantMonkey::GROGGY);
		else if (affect == AFFECT_KNOCKBACK)
		{
			Change_ReactionMode();
			return true;
		}
			
		return false;
	}

	else if (m_monNum == MON_MONKEY)
	{
		if (affect == AFFECT_FINCH)
			send_Monster_animation_packet(to_client, Monkey::FINCH);
		else if (affect == AFFECT_GROGGY)
			send_Monster_animation_packet(to_client, Monkey::GROGGY);
		else if (affect == AFFECT_KNOCKBACK)
		{
			Change_ReactionMode();
			return true;
		}

		return false;
	}

	else if (m_monNum == MON_SAILOR)
	{
		if (affect == AFFECT_FINCH)
			send_Monster_animation_packet(to_client, DrownedSailor::FINCH);
		else if (affect == AFFECT_GROGGY)
			send_Monster_animation_packet(to_client, DrownedSailor::GROGGY);
		else if (affect == AFFECT_KNOCKBACK)
		{
			Change_ReactionMode();
			return true;
		}

		return false;
	}

	return false;
}

void CMonster::Hurt_MonsterbyAI(const int& p_id, const int& damage)
{
	if (m_bIsRegen || m_bIsDead) return;

	/* 피격 당함 */
	if (m_iHp > ZERO_HP)
	{
		m_iHp -= damage;
	}

	/* Monster Dead */
	else if (m_iHp <= ZERO_HP)
	{
		m_iHp = ZERO_HP;
		m_bIsDead = true;

		Change_DeadMode();
		return;
	}

	// Monster View List 내의 유저들에게 해당 Monster의 변경된 stat을 알림.
	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{	
		send_Monster_Stat(raid, p_id, damage);
	}
}

void CMonster::Hurt_Vergos(const int& p_id, const int& damage, const char& affect)
{
	if (m_bIsRegen || m_bIsDead || !m_bIsConnect) return;

	/* 피격 당함 */
	if (m_iHp > ZERO_HP)
	{
		m_iHp -= damage;
	}

	/* Monster Dead */
	else if (m_iHp <= ZERO_HP)
	{
		m_iHp = ZERO_HP;
		m_bIsDead = true;

		/* Increase Raid Memeber Exp */
		for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", raid));
			if (pPlayer == nullptr) continue;

			pPlayer->m_iExp += m_iExp;

			/* Player Level Up */
			if (pPlayer->m_iExp >= pPlayer->m_iMaxExp)
			{
				pPlayer->m_iLevel	+= INIT_LEV;
				pPlayer->m_iExp		= INIT_EXP;
				pPlayer->m_iMaxExp	+= pPlayer->m_iLevel * INCREASE_EXP;
				pPlayer->m_iMaxHp	+= pPlayer->m_iLevel * INCREASE_HP;
				pPlayer->m_iHp		= pPlayer->m_iMaxHp;
				pPlayer->m_iMaxMp	+= pPlayer->m_iLevel * INCREASE_MP;
				pPlayer->m_iMp		= pPlayer->m_iMaxMp;
				pPlayer->m_iMinAtt	+= pPlayer->m_iLevel * INCREASE_ATT;
				pPlayer->m_iMaxAtt	+= pPlayer->m_iLevel * INCREASE_ATT;
			}
			pPlayer->send_player_stat(raid);
		}
		Change_DeadMode();
		return;
	}

	// Monster View List 내의 유저들에게 해당 Monster의 변경된 stat을 알림.
	for (const int& raid : *CObjMgr::GetInstance()->Get_RAIDLIST())
	{
		send_Monster_Stat(raid, p_id, damage);
	}
}

void CMonster::Change_AttackMode()
{
	/* Monster가 활성화되어 있지 않을 경우 활성화 */
	if (m_bIsDead || m_bIsRegen || m_bIsReaction) return;
	if (m_status != ST_ATTACK)
	{
		STATUS prev_state = m_status;
		if (true == atomic_compare_exchange_strong(&m_status, &prev_state, ST_ATTACK))
			Set_Start_Attack();
	}
}

void CMonster::Change_ChaseMode()
{
	/* Monster가 활성화되어 있지 않을 경우 활성화 */
	if (m_bIsDead || m_bIsRegen || m_bIsReaction) return;
	if (m_status != ST_CHASE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_CHASE);
	}
}

void CMonster::Change_ReactionMode()
{
	if (m_bIsDead || m_bIsRegen) return;
	if (m_status != ST_REACTION)
	{
		STATUS prev_state = m_status;
		if (true == atomic_compare_exchange_strong(&m_status, &prev_state, ST_REACTION))
			Set_Start_Reaction();
	}
}

void CMonster::Change_DeadMode()
{
	if (m_status != ST_DEAD)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_DEAD);
	}
}

float CMonster::Calculate_TargetDist(const _vec3& vPos)
{
	float dist = (vPos.x - m_vPos.x) * (vPos.x - m_vPos.x);
	dist += (vPos.y - m_vPos.y) * (vPos.y - m_vPos.y);
	dist += (vPos.z - m_vPos.z) * (vPos.z - m_vPos.z);

	return dist;
}

void CMonster::Play_Animation(const float& fTimeDelta)
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

void CMonster::Set_AnimationKey(const _uint& uiAniKey)
{
	if (m_uiNewAniIndex != uiAniKey)
	{
		m_uiNewAniIndex = uiAniKey;
		m_fBlendingTime = 1.0f;
		m_fBlendAnimationTime = m_fAnimationTime;
	}
}

bool CMonster::Is_AnimationSetEnd(const float& fTimeDelta)
{
	if ((m_fAnimationTime >= m_arrDuration[m_uiCurAniIndex] -
		static_cast<double>(Monster_Normal::TPS * ANIMA_INTERPOLATION * fTimeDelta)) &&
		(m_uiCurAniIndex == m_uiNewAniIndex))
	{
		return true;
	}

	return false;
}

void CMonster::Set_Stop_Attack(chrono::seconds t)
{
	/* 공격이 끝나면 추적 모드로 변경 */
	if (m_bIsAttack)
	{
		bool prev_state = m_bIsAttack;

		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, false))
		{
			add_timer(m_sNum, OP_MODE_CHASE_MONSTER, system_clock::now() + t);
		}
	}
}

void CMonster::Set_Start_Attack()
{
	if (!m_bIsAttack)
	{
		bool prev_state = m_bIsAttack;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, true);		
	}
}

void CMonster::Set_Start_Fight()
{
	if (!m_bIsFighting)
	{
		bool prev_state = m_bIsFighting;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsFighting), &prev_state, true);
	}
}

void CMonster::Set_Stop_Fight()
{
	if (m_bIsFighting)
	{
		bool prev_state = m_bIsFighting;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsFighting), &prev_state, false);
	}
}

void CMonster::Set_Start_Regen(chrono::seconds t)
{
	if (!m_bIsRegen)
	{
		bool prev_state = m_bIsRegen;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsRegen), &prev_state, true))
		{
			add_timer(m_sNum, OP_MODE_REGEN_MONSTER, system_clock::now() + t);
		}
	}
}

void CMonster::Set_Finish_Regen()
{
	if (m_bIsRegen)
	{
		bool prev_state = m_bIsRegen;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsRegen), &prev_state, false))
		{
			m_bIsDead = false;
			m_iHp = m_iMaxHp;
			m_iTargetNum = -1;
			m_vPos = m_vOriPos;
		}
	}
}

void CMonster::Set_Start_Reaction()
{
	if (!m_bIsReaction)
	{
		bool prev_state = m_bIsReaction;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, true);
	}
}

void CMonster::Set_Stop_Reaction(chrono::seconds t)
{
	if (m_bIsReaction)
	{
		bool prev_state = m_bIsReaction;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false))
		{
			m_vPos = m_vKnockbackPos;
			add_timer(m_sNum, OP_MODE_CHASE_MONSTER, system_clock::now() + t);
		}
	}
}

void CMonster::Init_AllStatus()
{
	bool prev_state = m_bIsAttack;
	atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, false);

	prev_state = m_bIsFighting;
	atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsFighting), &prev_state, false);

	prev_state = m_bIsShortAttack;
	atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsShortAttack), &prev_state, false);

	prev_state = m_bIsRushAttack;
	atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsRushAttack), &prev_state, false);

	prev_state = m_bIsReaction;
	atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsReaction), &prev_state, false);
}

void CMonster::active_monster()
{
	/* Monster가 활성화되어 있지 않을 경우 활성화 */
	if (m_status == ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE);
	}
}

void CMonster::nonActive_monster()
{
	if (m_status != ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		if (true == atomic_compare_exchange_strong(&m_status, &prev_state, ST_NONACTIVE))
			m_vTempPos = m_vPos;
	}
}

void CMonster::send_Monster_enter_packet(const int& to_client)
{
	sc_packet_monster_enter p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_ENTER;
	p.id = m_sNum;

	p.naviType = m_chStageId;
	p.o_type = m_type;
	p.mon_num = m_monNum;

	p.animIdx = Monster_Normal::WAIT;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.angleX = m_vAngle.x;
	p.angleY = m_vAngle.y;
	p.angleZ = m_vAngle.z;

	p.Hp = m_iHp;
	p.maxHp = m_iMaxHp;
	p.spd = m_fSpd;
	p.min_att = m_iMinAtt;
	p.max_att = m_iMaxAtt;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_move_packet(const int& to_client, const int& ani)
{
	sc_packet_move p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_MOVE;
	p.id = m_sNum;

	p.animIdx = ani;
	p.spd  = m_fSpd;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_NormalAttack(const int& to_client, const int& ani)
{
	sc_packet_monster_attack p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_ATTACK;
	p.id = m_sNum;

	p.animIdx = ani;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_RushAttack(const int& to_client, const int& ani)
{
	sc_packet_monster_rushAttack p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_RUSH;
	p.id = m_sNum;

	p.animIdx = ani;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_Stat(const int& to_client, const int& p_id, const int& damage)
{
	sc_packet_update_party p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_MONSTER_STAT;
	p.id	= m_sNum;

	p.hp	= m_iHp;
	p.maxHp = m_iExp;
	p.mp	= damage;
	p.maxMp = p_id;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_Dead(const int& to_client, const int& ani)
{
	sc_packet_animationIndex p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_DEAD;
	p.id = m_sNum;

	p.aniIdx = ani;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_animation_packet(const int& to_client, const int& ani)
{
	sc_packet_animationIndex p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ANIM_INDEX;
	p.id = m_sNum;

	p.aniIdx = ani;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_Knockback(const int& to_client, const int& ani)
{
	sc_packet_monster_knockback p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_NUCKBACK;
	p.id = m_sNum;

	p.animIdx = ani;

	p.posX = m_vKnockbackPos.x;
	p.posY = m_vKnockbackPos.y;
	p.posZ = m_vKnockbackPos.z;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_Effect(const int& to_client)
{
	sc_packet_monster_effect p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_EFFECT;

	p.mon_id = m_sNum;
	
	for (int i = 0; i < 3; ++i)
	{
		p.posX[i] = m_arrVergosSkillEffect[i].x;
		p.posZ[i] = m_arrVergosSkillEffect[i].y;
	}

	send_packet(to_client, &p);
}

DWORD CMonster::Release()
{
	return 0;
}
