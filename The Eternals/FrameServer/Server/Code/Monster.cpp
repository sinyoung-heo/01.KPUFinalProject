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

	/* �ش� Monster�� ���� �þ� ��� */
	v_lock.lock();
	unordered_set<int> old_viewlist = view_list;
	v_lock.unlock();

	/* Monster ������ ó�� */
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

	/* Monster Move */
	m_vPos += m_vDir * fTimeDelta;

	/* NaviMesh�� ��� ��� ������ X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)	
		return;
	
	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// ������ �� ��ġ������ viewlist (�þ� ���� �÷��̾� ����)
	unordered_set <int> new_viewlist;

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
				if (obj_num == m_sNum) continue;
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

	// new_vielist ��ȸ -> �÷��̾��� �þ� ��Ͽ� �־�� �� ���ο� Monster���� �߰�
	for (auto pl : new_viewlist)
	{
		// Monster �þ� ��Ͽ� ���� ���� ���� ó�� (���� �þ� ��Ͽ� ���ٸ�)
		if (0 == old_viewlist.count(pl))
		{
			v_lock.lock();
			view_list.insert(pl);
			v_lock.unlock();
		}

		/* ������ �þ� ��� ó�� */
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();

			/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��� */
			if (0 == pPlayer->view_list.count(m_sNum))
			{
				pPlayer->view_list.insert(m_sNum);
				pPlayer->v_lock.unlock();
				send_Monster_enter_packet(pl, m_sNum);
			}
			/* �� ������ �þ� ��� ���� ���� Monster�� ���� ��� -> ���� Monster ��ġ ���� */
			else
			{
				pPlayer->v_lock.unlock();
				send_Monster_move_packet(pl, m_sNum);
			}	
		}
	}

	// �̵� �� viewlist & �̵� �� viewlist �� -> �� �������� �þ� ��� ���� Monster ���� ���θ� ����.
	for (auto pl : old_viewlist)
	{
		// ���ŵ� �þ� ��Ͽ� ���� ������ ��� -> ����
		if (0 == new_viewlist.count(pl))
		{
			v_lock.lock();
			view_list.erase(pl);
			v_lock.unlock();
		}

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			/* �ش� ������ �þ� ��Ͽ� ���� Monster�� ������ ��� -> ���� */
			pPlayer->v_lock.lock();

			if (0 != pPlayer->view_list.count(m_sNum))
			{
				pPlayer->view_list.erase(m_sNum);
				pPlayer->v_lock.unlock();
				send_leave_packet(pl, m_sNum);
			}
			else pPlayer->v_lock.unlock();
		}
	}

	// Monster �þ� ���� �ƹ��� ���ٸ� NON ACTIVE�� ���� ����
	if (new_viewlist.empty() == true)
		nonActive_monster(m_sNum);
}

void CMonster::Move_ChaseMonster(const float& fTimeDelta)
{
}

DWORD CMonster::Release()
{
	return 0;
}
