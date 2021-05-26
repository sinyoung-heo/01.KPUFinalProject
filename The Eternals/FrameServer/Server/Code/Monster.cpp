#include "stdafx.h"
#include "Monster.h"
#include "Player.h"

CMonster::CMonster()
	:m_iHp(0), m_iMaxHp(0), m_iExp(0), m_iMinAtt(0), m_iMaxAtt(0), m_fSpd(0.f),
	m_iTargetNum(-1), m_bIsAttack(false), m_bIsShortAttack(true), m_bIsRegen(false),
	m_bIsRushAttack(false), m_bIsFighting(false), m_monNum(0), m_uiAnimIdx(0), m_bIsReaction(false),
	m_vNuckBackPos(_vec3(0.f)), m_eAttackDist(ATTACK_DIST::DIST_END)
{
}

CMonster::~CMonster()
{
}

void CMonster::Set_AnimDuration(double arr[])
{
	for (int i = 0; i < MAX_ANI; ++i)
	{
		if (m_uiNumAniIndex > i)
			m_arrDuration[i] = arr[i];
	}
}

void CMonster::Ready_Monster(const _vec3& pos, const _vec3& angle, const char& type, const char& num, const char& naviType, const int& hp, const int& att, const int& exp, const float& spd)
{
	/* NPC�� ���� �ʱ�ȭ */
	m_sNum += MON_NUM_START;
	int s_num = m_sNum;

	m_chStageId = naviType;

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
	if (m_bIsRegen)
		return NO_EVENT;

	if (fTimeDelta > 1.f)
		return NO_EVENT;

	Change_Animation(fTimeDelta);

	/* Calculate Animation frame */
	Set_AnimationKey(m_uiAnimIdx);
	Play_Animation(fTimeDelta * Monster_Normal::TPS);

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
		NuckBack_GiantMonkey(fTimeDelta);
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

void CMonster::Move_NormalMonster(const float& fTimeDelta)
{
	if (m_bIsDead || m_bIsRegen) return;

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	if (m_monNum == MON_CRAB)
		m_fSpd = CRAB_SPD;
	else
		m_fSpd = 1.f;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// �̵� ��: ���� ���� ��ȸ
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (obj_num == m_sNum) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Monster ������ ���� ó�� */
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

		/* �ش� NPC�� �̷� ��ġ ��ǥ ���� -> �̷� ��ġ��ǥ�� �ӽ� ������ ���� */
		m_vTempPos += m_vDir * 5.f;

		if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vTempPos, m_vOriPos))
		{
			m_vTempPos = m_vPos;
			return;
		}
	}

	/* Monster Move */
	m_vPos += m_vDir * m_fSpd * fTimeDelta;

	/* NaviMesh�� ��� ��� ������ X */
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
		

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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

	// Monster �þ� ���� �ƹ��� ���ٸ� NON ACTIVE�� ���� ����
	if (new_viewlist.empty() == true)
		nonActive_monster();
}

void CMonster::Chase_Crab(const float& fTimeDelta)
{
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = CRAB_CHASE_SPD;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_Target(this, pPlayer))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		int target_id = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", target_id));
		if (pTarget == nullptr) 		
			return;
		
		/* Ÿ���� �����ϱ� ���� �������� ���� */
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
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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

	/* NaviMesh�� ��� ��� ������ X */
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
	
	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = 3.0f;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, THROW_RANGE_MONKEY_END))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());
	
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* Ÿ���� �����ϱ� ���� �������� ���� */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* monster chase move -> arrive at player -> start to attack */

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((THROW_RANGE_MONKEY_START * THROW_RANGE_MONKEY_START) < fDist && fDist < (THROW_RANGE_MONKEY_END * THROW_RANGE_MONKEY_END))
		{
			m_bIsShortAttack = false;
			Change_AttackMode();
			return;
		}
		else if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) >= fDist)
		{
			m_bIsShortAttack = true;
			Change_AttackMode();
			return;
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh�� ��� ��� ������ X */
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

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = 4.0f;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_Target(this, pPlayer))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* Ÿ���� �����ϱ� ���� �������� ���� */
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
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh�� ��� ��� ������ X */
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

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = SAILOR_CHASE_SPD;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_Target(this, pPlayer))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* Ÿ���� �����ϱ� ���� �������� ���� */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist && fDist < (ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER))
		{
			m_bIsShortAttack = false;
			Change_AttackMode();
			return;
		}
		else if ((ATTACK_RANGE_SAILOR * ATTACK_RANGE_SAILOR) < fDist)
			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		else if ((ATTACK_RANGE_SAILOR * ATTACK_RANGE_SAILOR) >= fDist)
		{
			m_bIsShortAttack = true;
			Change_AttackMode();
			return;
		}

	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh�� ��� ��� ������ X */
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

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::RUN);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = 4.0f;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, THROW_RANGE_MONKEY_END))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* Ÿ���� �����ϱ� ���� �������� ���� */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
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
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh�� ��� ��� ������ X */
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

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = 4.0f;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_Target(this, pPlayer))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* Ÿ���� �����ϱ� ���� �������� ���� */
		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
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
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh�� ��� ��� ������ X */
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

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	m_fSpd = ARCHNE_CHASE_SPD;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �̵� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, ATTACK_RANGE_ARACHNE))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> Ÿ�� ���� */
	if (!old_targetList.empty())
	{
		if (m_iTargetNum < 0)
			m_iTargetNum = *(old_targetList.begin());

		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr)
			return;

		/* Ÿ���� �����ϱ� ���� �������� ���� */
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
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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
			nonActive_monster();
			Set_Stop_Fight();
		}
	}

	/* NaviMesh�� ��� ��� ������ X */
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

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// �̵� ��: ���� ���� ��ȸ -> ������ ���� �� new viewlist ���� ���
	for (auto& s : nearSectors)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						new_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// �̵� �Ŀ��� Monster �þ� ��� ���� "pl"(server number) ������ �����ִ� ���
		if (0 < new_viewlist.count(pl))
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					pPlayer->v_lock.unlock();
					/* �ش� �������� NPC�� ������ ���� ��ġ�� ���� */
					send_Monster_move_packet(pl, Monster_Normal::WALK);
				}
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� �������� ���� ��� */
				else
				{
					/* �ش� ������ �þ� ��Ͽ� ���� Monster ��� */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
			}
		}
		// �̵� �Ŀ� Monster �þ� ��� ���� "pl"(server number) ������ ���� ���
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
			if (pPlayer != nullptr)
			{
				/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� */
				pPlayer->v_lock.lock();
				if (0 < pPlayer->view_list.count(m_sNum))
				{
					/* �ش� ������ �þ� ��Ͽ��� ���� Monster ���� */
					pPlayer->view_list.erase(m_sNum);
					pPlayer->v_lock.unlock();
					send_leave_packet(pl, m_sNum);
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_Monster_enter_packet(pl);
				}
				/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
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
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;
	 
	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);

					// ���� ���� ���� ���� �ִ� ���� Ž���Ѵ�.
					if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer, ATTACK_RANGE_CRAB))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (!old_targetList.empty())
	{
		/* ���� �ִϸ��̼� ���� */
		m_uiAnimIdx = Monster_Normal::ATTACK;
		Set_AnimationKey(m_uiAnimIdx);

		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}		
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_Monkey(const float& fTimeDelta)
{
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((THROW_RANGE_MONKEY_START * THROW_RANGE_MONKEY_START) < fDist && fDist < (THROW_RANGE_MONKEY_END * THROW_RANGE_MONKEY_END))
		{
			m_bIsShortAttack = false;
		}

		/* ���� �ִϸ��̼� ���� */
		if (m_bIsShortAttack)
			m_uiAnimIdx = Monster_Normal::ATTACK;
		else
			m_uiAnimIdx = Monkey::ATTACK_THROW;

		Set_AnimationKey(m_uiAnimIdx);
		
		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;		
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_Cloder(const float& fTimeDelta)
{
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���� �ִϸ��̼� ���� */
		m_uiAnimIdx = rand() % 3 + 3;
		Set_AnimationKey(m_uiAnimIdx);

		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_DrownedSailor(const float& fTimeDelta)
{
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((ATTACK_RANGE_MONKEY * ATTACK_RANGE_MONKEY) < fDist && fDist < (ATTACK_RANGE_CLODER * ATTACK_RANGE_CLODER))
		{
			m_bIsShortAttack = false;
		}

		/* ���� �ִϸ��̼� ���� */
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

		/* ������ ��� �� ���� ��Ŷ ���� */
		if (m_bIsRushAttack) return;

		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_GiantBeetle(const float& fTimeDelta)
{
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���Ÿ� & �ٰŸ� ���� Ÿ�� ���� */
		float fDist = Calculate_TargetDist(pTarget->m_vPos);
		if ((RUSH_RANGE_GBEETEL_START * RUSH_RANGE_GBEETEL_START) < fDist && fDist < (RUSH_RANGE_GBEETEL_END * RUSH_RANGE_GBEETEL_END))
		{
			m_bIsShortAttack = false;
		}

		/* ���� �ִϸ��̼� ���� */
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

		/* ������ ��� �� ���� ��Ŷ ���� */
		if (m_bIsRushAttack) return;

		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_GiantMonkey(const float& fTimeDelta)
{
	if (m_bIsReaction) return;
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		/* ���� Ÿ�� ���� */
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
		
		/* ������ ��� �� ���� ��Ŷ ���� */
		if (m_bIsRushAttack) return;

		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Change_ChaseMode();
		return;
	}
}

void CMonster::Attack_CraftyArachne(const float& fTimeDelta)
{
	// ���� ���� �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (m_iTargetNum != -1)
	{
		CPlayer* pTarget = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", m_iTargetNum));
		if (pTarget == nullptr) return;

		m_vDir = pTarget->m_vPos - m_vPos;
		m_vDir.Normalize();

		m_uiAnimIdx = rand() % 4 + 2;
		Set_AnimationKey(m_uiAnimIdx);

		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;		
				send_Monster_NormalAttack(pl, m_uiAnimIdx);
			}
		}
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	for (auto pl : old_viewlist)
	{
		/* ������ ��� ó�� */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{		
			if (m_uiAnimIdx == DrownedSailor::ATTACK_RUSH)
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	for (auto pl : old_viewlist)
	{
		/* ������ ��� ó�� */
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	for (auto pl : old_viewlist)
	{
		/* ������ ��� ó�� */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			if (m_uiAnimIdx == GiantMonkey::ATTACK_JUMPING || m_uiAnimIdx == GiantMonkey::ATTACK_COMBO)
			{
				send_Monster_RushAttack(pl, m_uiAnimIdx);
			}
		}
	}
}

void CMonster::NuckBack_GiantMonkey(const float& fTimeDelta)
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �˹� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;
	unordered_set <int> old_targetList;

	// �˹� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// �˹� ���� ��ġ ����
	m_vNuckBackPos = m_vPos + (-1.f * m_vDir) * NUCKBACK_DIST;

	for (auto pl : old_viewlist)
	{
		/* ������ ��� ó�� */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{	
			send_Monster_NuckBack(pl, GiantMonkey::DOWN);	
		}
	}

	m_uiAnimIdx = GiantMonkey::DOWN;
	Set_AnimationKey(GiantMonkey::DOWN);
}

void CMonster::Dead_Crab(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, Crab::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = Crab::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_Monkey(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
	if (Monkey::DEATH == m_uiAnimIdx)
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, Monkey::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = Monkey::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_Cloder(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, Cloder::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = Cloder::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_DrownedSailor(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, DrownedSailor::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = DrownedSailor::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_GiantBeetle(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, GiantBeetle::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = GiantBeetle::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_GiantMonkey(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
	if (GiantMonkey::DEATH == m_uiAnimIdx)
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, GiantMonkey::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = GiantMonkey::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Dead_CraftyArachne(const float& fTimeDelta)
{
	if (m_bIsRegen) return;

	// �ִϸ��̼��� ���� ������� �ʾ��� ��� -> ���� ���� ��Ŷ ���� X
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

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �״� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// ���� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
	for (auto pl : old_viewlist)
	{
		send_Monster_Dead(pl, CraftyArachne::DEATH);

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));

		pPlayer->v_lock.lock();
		pPlayer->view_list.erase(m_sNum);
		pPlayer->v_lock.unlock();
	}

	/* �ִϸ��̼� ���� */
	m_uiAnimIdx = CraftyArachne::DEATH;
	Set_AnimationKey(m_uiAnimIdx);
}

void CMonster::Hurt_Monster(const int& p_id, const int& damage, const char& affect)
{
	if (m_bIsRegen || m_bIsDead) return;

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �ǰݴ��ϱ� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// �ǰݴ��ϱ� ��: ���� ���� ��ȸ (���� �þ� �ľ�)
	for (auto& s : oldnearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* ������ ��� ó�� */
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pPlayer->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(this, pPlayer))
						old_viewlist.insert(obj_num);
				}
			}
		}
	}

	/* �ǰ� ���� */
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
		}
		send_player_stat(p_id, p_id);

		Change_DeadMode();
		return;
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ����� stat�� �˸�.
	for (auto pl : old_viewlist)
	{
		/* ������ ��� ó�� */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			if (m_bIsDead) return;
			send_Monster_Stat(pl);

			/* Affect */
			if (m_monNum == MON_GMONKEY && !m_bIsAttack)
			{
				if (affect == AFFECT_FINCH && !m_bIsReaction)
					send_Monster_animation_packet(pl, GiantMonkey::FINCH);
				else if (affect == AFFECT_GROGGY && !m_bIsReaction)
					send_Monster_animation_packet(pl, GiantMonkey::GROGGY);
				else if (affect == AFFECT_NUCKBACK)
				{
					Change_ReactionMode();
					return;
				}
			}
		}
	}

	/* Monster Attack Target ���� */
	m_iTargetNum = p_id;

	/* ���� ���·� ���� */
	if (m_bIsFighting == false)
	{
		Set_Start_Fight();
		Change_ChaseMode();
	}
}

void CMonster::Change_AttackMode()
{
	/* Monster�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
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
	/* Monster�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
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

void CMonster::Play_Animation(float fTimeDelta)
{
	if (m_uiCurAniIndex >= m_uiNumAniIndex) return;

	/*__________________________________________________________________________________________________________
	[ �ִϸ��̼��� ��� �ݺ��ǵ��� fmod ���� ]
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
	[ 3DMax �󿡼��� Frame ��� ]
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
	/* ������ ������ ���� ���� ���� */
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
			m_vPos = m_vNuckBackPos;
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
}

void CMonster::active_monster()
{
	/* Monster�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
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

void CMonster::send_Monster_enter_packet(int to_client)
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

void CMonster::send_Monster_move_packet(int to_client, int ani)
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

void CMonster::send_Monster_NormalAttack(int to_client,int ani)
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

void CMonster::send_Monster_RushAttack(int to_client, int ani)
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

void CMonster::send_Monster_Stat(int to_client)
{
	sc_packet_stat_change p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_STAT;
	p.id = m_sNum;

	p.hp = m_iHp;
	p.mp = 0;
	p.exp = m_iExp;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_Dead(int to_client, int ani)
{
	sc_packet_animationIndex p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_DEAD;
	p.id = m_sNum;

	p.aniIdx = ani;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_animation_packet(int to_client, int ani)
{
	sc_packet_animationIndex p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ANIM_INDEX;
	p.id = m_sNum;

	p.aniIdx = ani;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_NuckBack(int to_client, int ani)
{
	sc_packet_monster_nuckback p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_NUCKBACK;
	p.id = m_sNum;

	p.animIdx = ani;

	p.posX = m_vNuckBackPos.x;
	p.posY = m_vNuckBackPos.y;
	p.posZ = m_vNuckBackPos.z;

	send_packet(to_client, &p);
}

DWORD CMonster::Release()
{
	return 0;
}
