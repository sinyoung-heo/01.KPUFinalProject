#include "stdafx.h"
#include "Monster.h"
#include "Player.h"

CMonster::CMonster()
	:Hp(0), maxHp(0), targetNum(-1), pre_state(STATUS::ST_END)
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
		break;
	case STATUS::ST_WAIT:
		break;
	case STATUS::ST_IDLE:
		break;
	case STATUS::ST_ATTACK:
		break;
	case STATUS::ST_DEAD:
		break;
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

		/* monster chase move */
		if (!CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, pTarget->m_vPos))
			m_vPos += m_vDir * fTimeDelta;
		else
			nonActive_monster(m_sNum);	   /// ���߿� ���� ���� �ٲٱ�.....
	}
	/* Ÿ��(���� ���)�� �������� ���� ��� -> ������ ��ġ�� ���ư� */
	else
	{
		m_vDir = m_vOriPos - m_vPos;
		m_vDir.Normalize();

		/* monster chase move */
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

DWORD CMonster::Release()
{
	return 0;
}
