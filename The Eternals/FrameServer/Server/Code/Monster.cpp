#include "stdafx.h"
#include "Monster.h"
#include "Player.h"

CMonster::CMonster()
	:Hp(0), maxHp(0), Exp(0), att(0), spd(0.f),
	targetNum(-1), m_bIsAttack(false), m_bIsComeBack(false)
{
}

CMonster::~CMonster()
{
}

int CMonster::Update_Monster(const float& fTimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > 1.f)
		return NO_EVENT;

	Change_Animation(fTimeDelta);

	return NO_EVENT;
}

void CMonster::Change_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{
	/* ���� */
	case STATUS::ST_ACTIVE:
	{
		if (m_monNum == MON_NORMAL)
		{
			Move_NormalMonster(fTimeDelta);
		}
		else if (m_monNum == MON_CHASE)
		{
			Move_ChaseMonster(fTimeDelta);
		}
		else if (m_monNum == MON_BOSS)
		{

		}
	}
	break;

	case STATUS::ST_NONACTIVE:	
	{
		m_bIsComeBack = false;
	}
	break;
	case STATUS::ST_WAIT:
		break;
	case STATUS::ST_IDLE:
		break;
	case STATUS::ST_ATTACK:
	{
		Attack_Monster(fTimeDelta);
	}
	break;
	case STATUS::ST_DEAD:
		break;
	}
}

void CMonster::Move_ComeBack(const float& fTimeDelta)
{
	m_bIsComeBack = true;

	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
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

	/* Monster ������ ó�� */
	m_vDir = m_vOriPos - m_vPos;
	m_vDir.Normalize();

	/* monster return home position */
	if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vOriPos))
		m_vPos += m_vDir * fTimeDelta;
	else
		nonActive_monster(m_sNum);
	

	/* NaviMesh�� ��� ��� ������ X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
		return;

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
					send_Monster_move_packet(pl);
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
					send_Monster_move_packet(pl);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}

	}	
}

void CMonster::Move_NormalMonster(const float& fTimeDelta)
{
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
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

	/* Monster ������ ó�� */
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
		m_vTempPos += m_vDir * 3.f;
	}
	
	/* Monster Move */
	m_vPos += m_vDir * fTimeDelta * 3.f;

	/* NaviMesh�� ��� ��� ������ X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
		return;

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
					send_Monster_move_packet(pl);
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
					send_Monster_move_packet(pl);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}

	}

	// Monster �þ� ���� �ƹ��� ���ٸ� NON ACTIVE�� ���� ����
	if (new_viewlist.empty() == true)
		nonActive_monster(m_sNum);
}

void CMonster::Move_ChaseMonster(const float& fTimeDelta)
{
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �����̱� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
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
			m_vPos += m_vDir * fTimeDelta;
		else
			Change_AttackMode();
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
			nonActive_monster(m_sNum);
	}

	/* NaviMesh�� ��� ��� ������ X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)
		return;
	
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
					send_Monster_move_packet(pl);
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
					send_Monster_move_packet(pl);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}

	// Monster �þ� ���� �ƹ��� ���ٸ� NON ACTIVE�� ���� ����
	if (new_viewlist.empty() == true)
		nonActive_monster(m_sNum);
}

void CMonster::Attack_Monster(const float& fTimeDelta)
{
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;
	 
	// ���� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
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
					if (CObjMgr::GetInstance()->Is_Monster_AttackTarget(this, pPlayer))
						old_targetList.insert(obj_num);
				}
			}
		}
	}

	/* Ÿ��(���� ���)�� ������ ��� -> ���� ���� ���� �˸��� */
	if (!old_targetList.empty())
	{
		// Monster View List ���� �����鿡�� �ش� Monster�� ���� ������ �˸�.
		for (auto pl : old_viewlist)
		{
			/* ������ ��� ó�� */
			if (true == CObjMgr::GetInstance()->Is_Player(pl))
			{
				if (!m_bIsAttack) return;
				cout << "���� ���� ��Ŷ ����" << endl;
				send_Monster_NormalAttack(pl);
			}
		}		
		// �ֺ� �������� monster_attack_start�� �˷ȴٸ� ��� ���� ���� -> ���� �ð� �� �����
		Set_Stop_Attack();
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		Move_ComeBack(fTimeDelta);
	}
}

void CMonster::Hurt_Monster(const int& damage)
{
	/* �ش� Monster�� ���� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// �ǰݴ��ϱ� �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
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
	if (Hp >= 0)
	{
		Hp -= damage;
		if (Hp < 0)
			Hp = 0;
	}

	// Monster View List ���� �����鿡�� �ش� Monster�� ����� stat�� �˸�.
	for (auto pl : old_viewlist)
	{
		/* ������ ��� ó�� */
		if (true == CObjMgr::GetInstance()->Is_Player(pl))
		{
			if (m_bIsDead) return;
			cout << "���� �ǰݴ��� ��Ŷ ����" << endl;
			send_Monster_Stat(pl);
		}
	}
}

void CMonster::Change_AttackMode()
{
	/* Monster�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
	if (m_status != ST_ATTACK)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ATTACK);
	}

	Set_Start_Attack();
}

void CMonster::Set_Stop_Attack()
{
	if (m_bIsAttack)
	{
		bool prev_state = m_bIsAttack;

		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, false))
			add_timer(m_sNum, OP_MODE_ATTACK_MONSTER, system_clock::now() + 3s);
	}
}

void CMonster::Set_Start_Attack()
{
	if (!m_bIsAttack && !m_bIsComeBack)
	{
		bool prev_state = m_bIsAttack;
		atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsAttack), &prev_state, true);		
	}
}

void CMonster::send_Monster_enter_packet(int to_client)
{
	sc_packet_monster_enter p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_ENTER;
	p.id = m_sNum;

	c_lock.lock();
	strncpy_s(p.name, m_ID, strlen(m_ID));
	strncpy_s(p.naviType, m_naviType, strlen(m_naviType));
	c_lock.unlock();

	p.mon_num = m_monNum;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.angleX = m_vAngle.x;
	p.angleY = m_vAngle.y;
	p.angleZ = m_vAngle.z;

	p.Hp = Hp;
	p.maxHp = maxHp;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_move_packet(int to_client)
{
	sc_packet_move p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_MOVE;
	p.id = m_sNum;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_NormalAttack(int to_client)
{
	sc_packet_monster_attack p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_ATTACK;
	p.id = m_sNum;

	send_packet(to_client, &p);
}

void CMonster::send_Monster_Stat(int to_client)
{
	sc_packet_stat_change p;

	p.size = sizeof(p);
	p.type = SC_PACKET_MONSTER_STAT;

	p.id = m_sNum;
	p.hp = Hp;
	p.mp = 0;
	p.exp = Exp;

	send_packet(to_client, &p);
}

DWORD CMonster::Release()
{
	return 0;
}
