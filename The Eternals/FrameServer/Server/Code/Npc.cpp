#include "stdafx.h"
#include "Npc.h"
#include "Player.h"

CNpc::CNpc()
	:m_npcNum(0), m_fSpd(1.f), m_bIsMove(false), m_bIsDirSelect(false)
{
}

CNpc::~CNpc()
{
}

void CNpc::Set_AnimDuration(double arr[])
{
	for (int i = 0; i < MAX_ANI; ++i)
	{
		if (m_uiNumAniIndex > i)
			m_arrDuration[i] = arr[i];
	}
}

void CNpc::Set_AnimationKey(const _uint& uiAniKey)
{
	if (m_uiNewAniIndex != uiAniKey)
	{
		m_uiNewAniIndex = uiAniKey;
		m_fBlendingTime = 1.0f;
		m_fBlendAnimationTime = m_fAnimationTime;
	}
}

void CNpc::Set_Start_Move(chrono::seconds t)
{
	if (!m_bIsMove)
	{
		bool prev_state = m_bIsMove;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsMove), &prev_state, true))
		{
			add_timer(m_sNum, OP_ACTIVE_NPC, system_clock::now() + t);
		}
	}
}

void CNpc::Set_Stop_Move()
{
	if (m_bIsMove)
	{
		bool prev_state = m_bIsMove;
		if (true == atomic_compare_exchange_strong(reinterpret_cast<volatile atomic_bool*>(&m_bIsMove), &prev_state, false))
		{
			nonActive_npc();
		}
	}
}

void CNpc::active_npc()
{
	/* NPC가 활성화되어 있지 않을 경우 활성화 */
	if (m_status != ST_ACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE);
	}
}

void CNpc::nonActive_npc()
{
	/* NPC가 활성화되어 있지 않을 경우 활성화 */
	if (m_status != ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_NONACTIVE);
	}
}

void CNpc::wakeUp_npc()
{
	/* NPC가 활성화되어 있지 않을 경우 활성화 */
	if (m_status == ST_END)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_NONACTIVE);
	}
}

void CNpc::Ready_NPC(const _vec3& pos, const _vec3& angle, const char& type, const char& num, const char& naviType)
{
	/* NPC의 정보 초기화 */
	m_sNum += NPC_NUM_START;
	int s_num = m_sNum;

	m_chStageId = naviType;

	m_bIsConnect = true;
	m_bIsDead = false;

	m_vPos = pos;
	m_vTempPos = m_vPos;
	m_vDir = _vec3(0.f, 0.f, 1.f);
	m_vAngle = angle;
	m_type = type;
	m_npcNum = num;
	m_status = STATUS::ST_END;

	CSectorMgr::GetInstance()->Enter_ClientInSector(s_num, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));
	CObjMgr::GetInstance()->Add_GameObject(L"NPC", this, s_num);
}

int CNpc::Update_NPC(const float& fTimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > 1.f)
		return NO_EVENT;

	Change_Animation(fTimeDelta);

	/* Calculate Animation frame */
	Set_AnimationKey(m_uiAnimIdx);
	Play_Animation(fTimeDelta * NPC_TYPE::TPS);

	return NO_EVENT;
}

void CNpc::Change_Animation(const float& fTimeDelta)
{
	/* MOVE NPC - Walker */
	if (m_npcNum == NPC_CHICKEN || m_npcNum == NPC_CAT || m_npcNum == NPC_AMAN_BOY)
	{
		Change_Walker_Animation(fTimeDelta);
	}
	/* NPC - Assistant */
	else if (m_npcNum == NPC_POPORI_BOY)
	{
		Change_Assistant_Animation(fTimeDelta);
	}
	/* NPC - Merchant */
	else if (m_npcNum == NPC_POPORI_MERCHANT || m_npcNum == NPC_BARAKA_MERCHANT || m_npcNum == NPC_BARAKA_MYSTELLIUM)
	{
		Change_Merchant_Animation(fTimeDelta);
	}
	/* NPC - Quest NPC */
	else if (m_npcNum == NPC_CASTANIC_LSMITH)
	{
		Change_QuestNPC_Animation(fTimeDelta);
	}
}

void CNpc::Change_Walker_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = NPC_TYPE::WALK;

		if (m_bIsDirSelect)
		{
			if (false == CCollisionMgr::GetInstance()->Is_Arrive(m_vPos, m_vTempPos))
				m_vPos += m_vDir * m_fSpd * fTimeDelta;
			else
			{
				m_vPos = m_vTempPos;
				Set_Stop_Move();
			}
		}
		else
			Move_Walker_NPC(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_uiAnimIdx = NPC_TYPE::WAIT;

		m_bIsDirSelect = false;
		Set_Start_Move();
	}
	break;

	}
}

void CNpc::Change_Assistant_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = NPC_TYPE::IDLE;
		Idle_Assistant_NPC(fTimeDelta, NPC_TYPE::IDLE);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_uiAnimIdx = NPC_TYPE::WAIT;
		Set_Start_Move(10s);
	}
	break;

	}
}

void CNpc::Change_Merchant_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = NPC_MERCHANT_TYPE::GREET;
		Greet_Merchant_Animation(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_uiAnimIdx = NPC_TYPE::WAIT;
		Set_Start_Move(10s);
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = NPC_MERCHANT_TYPE::TALK;
	}
	break;

	}
}

void CNpc::Change_QuestNPC_Animation(const float& fTimeDelta)
{
	switch (m_status)
	{

	case STATUS::ST_ACTIVE:
	{
		m_uiAnimIdx = NPC_QUEST_TYPE::GREET;
		Greet_QuestNPC_Animation(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		m_uiAnimIdx = NPC_TYPE::WAIT;
		Set_Start_Move(10s);
	}
	break;

	case STATUS::ST_CHASE:
	{
		m_uiAnimIdx = NPC_QUEST_TYPE::TALK;
	}
	break;

	}
}

void CNpc::Move_Walker_NPC(const float& fTimeDelta)
{
	if (m_bIsDirSelect) return;

	m_fSpd = 1.f;

	/* 해당 NPC의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 이동 전: 인접 섹터 순회
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
				}
			}
		}
	}

	if (!m_bIsDirSelect)
	{
		/* NPC 움직임 처리 */
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
		m_vTempPos = m_vPos + m_vDir * 3.f;
		m_bIsDirSelect = true;
	}

	/* NaviMesh를 벗어날 경우 움직임 X */
	if (CNaviMesh::GetInstance()->Get_CurrentPositionCellIndex(m_vTempPos) == -1)
	{
		m_vTempPos = m_vPos;
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

	// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 NPC 존재 여부를 결정.
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
					send_NPC_move_packet(pl, NPC_TYPE::WALK);
				}
				/* 해당 유저의 시야 목록에 현재 Monster가 존재하지 않을 경우 */
				else
				{
					/* 해당 유저의 시야 목록에 현재 Monster 등록 */
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_NPC_enter_packet(pl);
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

	// new_vielist 순회 -> 플레이어의 시야 목록에 있어야 할 새로운 npc들을 추가
	for (auto pl : new_viewlist)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
		if (pPlayer != nullptr)
		{
			pPlayer->v_lock.lock();
			if (0 == pPlayer->view_list.count(pl))
			{
				/* 각 유저의 시야 목록 내에 현재 NPC가 없을 경우 -> 현재 NPC 등록 */
				if (0 == pPlayer->view_list.count(m_sNum))
				{
					pPlayer->view_list.insert(m_sNum);
					pPlayer->v_lock.unlock();
					send_NPC_enter_packet(pl);
				}
				/* 각 유저의 시야 목록 내에 현재 NPC가 있을 경우 -> 현재 NPC 위치 전송 */
				else
				{
					pPlayer->v_lock.unlock();
					send_NPC_move_packet(pl, NPC_TYPE::WALK);
				}
			}
			else
				pPlayer->v_lock.unlock();
		}
	}
}

void CNpc::Idle_Assistant_NPC(const float& fTimeDelta, int animIdx)
{
	/* 해당 NPC의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 이동 전: 인접 섹터 순회
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
				}
			}
		}
	}

	for (int server_num : old_viewlist)
	{
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pUser == nullptr) continue;
			if (!pUser->m_bIsConnect) continue;

			send_NPC_animation_packet(server_num, animIdx);
		}
	}

	Set_Stop_Move();
}

void CNpc::Greet_Merchant_Animation(const float& fTimeDelta)
{
	/* 해당 NPC의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 이동 전: 인접 섹터 순회
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
				}
			}
		}
	}

	for (int server_num : old_viewlist)
	{
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pUser == nullptr) continue;
			if (!pUser->m_bIsConnect) continue;

			send_NPC_animation_packet(server_num, NPC_MERCHANT_TYPE::GREET);
		}
	}

	Set_Stop_Move();
}

void CNpc::Greet_QuestNPC_Animation(const float& fTimeDelta)
{
	/* 해당 NPC의 원래 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = m_vPos.x;
	ori_y = m_vPos.y;
	ori_z = m_vPos.z;

	// 움직이기 전 위치에서의 viewlist (시야 내에 플레이어 저장)
	unordered_set<pair<int, int>> oldnearSector;
	oldnearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&oldnearSector, (int)ori_x, (int)ori_z);

	unordered_set <int> old_viewlist;

	// 이동 전: 인접 섹터 순회
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
				}
			}
		}
	}

	for (int server_num : old_viewlist)
	{
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pUser == nullptr) continue;
			if (!pUser->m_bIsConnect) continue;

			send_NPC_animation_packet(server_num, NPC_QUEST_TYPE::GREET);
		}
	}

	Set_Stop_Move();
}

void CNpc::Play_Animation(float fTimeDelta)
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

bool CNpc::Is_AnimationSetEnd(const float& fTimeDelta)
{
	if ((m_fAnimationTime >= m_arrDuration[m_uiCurAniIndex] -
		static_cast<double>(Monster_Normal::TPS * ANIMA_INTERPOLATION * fTimeDelta)) &&
		(m_uiCurAniIndex == m_uiNewAniIndex))
	{
		return true;
	}

	return false;
}

void CNpc::send_NPC_enter_packet(int to_client)
{
	sc_packet_npc_enter p;

	p.size = sizeof(p);
	p.type = SC_PACKET_NPC_ENTER;
	p.id = m_sNum;

	p.o_type = m_type;
	p.npcNum = m_npcNum;

	p.naviType = m_chStageId;

	p.posX = m_vPos.x;
	p.posY = m_vPos.y;
	p.posZ = m_vPos.z;

	p.angleX = m_vAngle.x;
	p.angleY = m_vAngle.y;
	p.angleZ = m_vAngle.z;

	send_packet(to_client, &p);
}

void CNpc::send_NPC_move_packet(int to_client, int ani)
{
	sc_packet_move p;

	p.size = sizeof(p);
	p.type = SC_PACKET_NPC_MOVE;
	p.id = m_sNum;

	p.animIdx = ani;
	p.spd = m_fSpd;

	p.posX = m_vTempPos.x;
	p.posY = m_vTempPos.y;
	p.posZ = m_vTempPos.z;

	p.dirX = m_vDir.x;
	p.dirY = m_vDir.y;
	p.dirZ = m_vDir.z;

	send_packet(to_client, &p);
}

void CNpc::send_NPC_animation_packet(int to_client, int ani)
{
	sc_packet_animationIndex p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ANIM_INDEX;
	p.id = m_sNum;

	p.aniIdx = ani;

	send_packet(to_client, &p);
}

DWORD CNpc::Release()
{
	return 0;
}
