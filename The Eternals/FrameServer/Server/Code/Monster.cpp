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
	/* 정찰 */
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
	/* 해당 Monster의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	/* 해당 Monster의 원래 시야 목록 */
	v_lock.lock();
	unordered_set<int> old_viewlist = view_list;
	v_lock.unlock();

	/* Monster 움직임 처리 */
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

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vPos) == -1)	
		return;
	
	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(m_sNum, (int)ori_z, (int)ori_x, (int)(m_vPos.z), (int)(m_vPos.x));

	unordered_set<pair<int, int>> nearSectors;
	nearSectors.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSectors, (int)(m_vPos.x), (int)(m_vPos.z));

	// 움직인 후 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set <int> new_viewlist;

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
				if (obj_num == m_sNum) continue;
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

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 Monster들을 추가
	for (auto pl : new_viewlist)
	{
		// Monster 시야 목록에 새로 들어온 유저 처리 (이전 시야 목록에 없다면)
		if (0 == old_viewlist.count(pl))
		{
			v_lock.lock();
			view_list.insert(pl);
			v_lock.unlock();
		}

		/* 유저의 시야 목록 처리 */
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();

			/* 각 유저의 시야 목록 내에 현재 Monster가 없을 경우 -> 현재 Monster 등록 */
			if (0 == pPlayer->view_list.count(m_sNum))
			{
				pPlayer->view_list.insert(m_sNum);
				pPlayer->v_lock.unlock();
				send_Monster_enter_packet(pl, m_sNum);
			}
			/* 각 유저의 시야 목록 내에 현재 Monster가 있을 경우 -> 현재 Monster 위치 전송 */
			else
			{
				pPlayer->v_lock.unlock();
				send_Monster_move_packet(pl, m_sNum);
			}	
		}
	}

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 Monster 존재 여부를 결정.
	for (auto pl : old_viewlist)
	{
		// 갱신된 시야 목록에 없는 유저일 경우 -> 삭제
		if (0 == new_viewlist.count(pl))
		{
			v_lock.lock();
			view_list.erase(pl);
			v_lock.unlock();
		}

		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			/* 해당 유저의 시야 목록에 현재 Monster가 존재할 경우 -> 삭제 */
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

	// Monster 시야 내에 아무도 없다면 NON ACTIVE로 상태 변경
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
