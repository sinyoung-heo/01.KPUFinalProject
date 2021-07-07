#include "stdafx.h"
#include "Ai.h"
#include "Player.h"

CAi::CAi()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0), m_fSpd(0.f),
    m_iMp(0), m_iMaxMp(0), m_iMaxAtt(0), m_iMinAtt(0.f), m_bIsMove(false),
    m_bIsAttackStance(false), m_bIsPartyState(false), m_iPartyNumber(-1)
{

}

CAi::~CAi()
{
}

void CAi::Ready_AI(const char& chJob, const char& chWeaponType, const char& chStageID, const _vec3& vPos)
{
    m_sNum += AI_NUM_START;
    int s_num = m_sNum;

    m_bIsConnect        = true;
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
    m_vDir              = _vec3(0.f, 0.f, 1.f);
    m_vAngle            = _vec3(0.f, 0.f, 0.f);
	m_status			= STATUS::ST_NONACTIVE;

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
	if (fTimeDelta > 1.f)
		return NO_EVENT;

	Change_Animation(fTimeDelta);

	return NO_EVENT;
}

void CAi::active_AI()
{
	if (m_status == ST_NONACTIVE)
	{
		STATUS prev_state = m_status;
		atomic_compare_exchange_strong(&m_status, &prev_state, ST_ACTIVE);
	}
}

void CAi::Change_Animation(const float& fTimeDelta)
{
	switch (m_type)
	{
	case PC_GLADIATOR:
	{

	}
	break;

	case PC_ARCHER:
	{
		Change_Archer_Animation(fTimeDelta);
	}
	break;

	case PC_PRIEST:
	{

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
		m_iAniIdx = Archer::ATTACK_RUN;
		process_move_ai(fTimeDelta);
	}
	break;

	case STATUS::ST_NONACTIVE:
	{
		
	}
	break;

	case STATUS::ST_CHASE:
	{
		
	}
	break;

	case STATUS::ST_ATTACK:
	{
		
	}
	break;

	case STATUS::ST_REACTION:
	{
		
	}
	break;

	case STATUS::ST_DEAD:
	{
		
	}
	break;
	}
}

void CAi::process_move_ai(const float& fTimeDelta)
{
	/* 방향 선정 */
	if (!m_bIsMove)
	{
		/* 해당 NPC의 원래 위치값 */
		float ori_x, ori_y, ori_z;
		ori_x = m_vPos.x;
		ori_y = m_vPos.y;
		ori_z = m_vPos.z;

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

		/* AI 움직임 처리 */
		switch (rand() % 3)
		{
		case 0: m_vDir = _vec3(0.f, 0.f, 1.f); break;	
		case 1: m_vDir = _vec3(1.f, 0.f, 1.f); break;
		case 2: m_vDir = _vec3(-1.f, 0.f, 1.f); break;
		}

		/* 해당 AI의 미래 위치 좌표 산출 -> 미래 위치좌표는 임시 변수에 저장 */
		_vec2 coll_pos = _vec2(0.f);

		if (CCollisionMgr::GetInstance()->Is_DeadReckoning(m_vPos, m_vDir, &coll_pos, m_chStageId))
		{
			m_vTempPos.x = coll_pos.x;
			m_vTempPos.y = 0.f;
			m_vTempPos.z = coll_pos.y;

			m_bIsMove = true;
		}
		else
		{
			m_vTempPos.x = m_vPos.x;
			m_vTempPos.y = m_vPos.y;
			m_vTempPos.z = m_vPos.z;
		}

		m_vDir = m_vTempPos - m_vPos;
		m_vDir.Normalize();

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
							new_viewlist.insert(obj_num);
					}
				}
			}
		}

		// 이동 전 viewlist & 이동 후 viewlist 비교 -> 각 유저들의 시야 목록 내에 NPC 존재 여부를 결정.
		for (auto pl : old_viewlist)
		{
			// 이동 후에도 AI 시야 목록 내에 "pl"(server number) 유저가 남아있는 경우
			if (0 < new_viewlist.count(pl))
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
				if (pPlayer != nullptr)
				{
					/* 해당 유저의 시야 목록에 현재 AI가 존재할 경우 */
					pPlayer->v_lock.lock();
					if (0 < pPlayer->view_list.count(m_sNum))
					{
						pPlayer->v_lock.unlock();
						/* 해당 유저에게 AI가 움직인 후의 위치를 전송 */
						send_AI_move_packet(pl);
					}
					/* 해당 유저의 시야 목록에 현재 AI가 존재하지 않을 경우 */
					else
					{
						/* 해당 유저의 시야 목록에 현재 AI 등록 */
						pPlayer->view_list.insert(m_sNum);
						pPlayer->v_lock.unlock();
						send_AI_enter_packet(pl);
					}
				}
			}
			// 이동 후에 AI 시야 목록 내에 "pl"(server number) 유저가 없는 경우
			else
			{
				CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", pl));
				if (pPlayer != nullptr)
				{
					/* 해당 유저의 시야 목록에 현재 AI가 존재할 경우 */
					pPlayer->v_lock.lock();
					if (0 < pPlayer->view_list.count(m_sNum))
					{
						/* 해당 유저의 시야 목록에서 현재 AI 삭제 */
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
		for (auto& pl : new_viewlist)
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
						send_AI_enter_packet(pl);
					}
					/* 각 유저의 시야 목록 내에 현재 NPC가 있을 경우 -> 현재 NPC 위치 전송 */
					else
					{
						pPlayer->v_lock.unlock();
						send_AI_move_packet(pl);
					}
				}
				else
					pPlayer->v_lock.unlock();
			}
		}
	}
	else
	{
	if (false == CCollisionMgr::GetInstance()->Is_InMoveLimit(m_vPos, m_vTempPos, 30.f))
		{
			m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Archer::OTHERS_OFFSET;
			m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
			m_fSpd = LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);

			m_vDir = m_vTempPos - m_vPos;
			m_vDir.Normalize();

			m_vPos += m_vDir * m_fSpd * fTimeDelta;
		}
		else m_bIsMove = false;	
	}

}

void CAi::process_disconnect_ai()
{
	/* sector에서 해당 플레이어 지우기 */
	CSectorMgr::GetInstance()->Leave_ClientInSector(m_sNum, (int)(m_vPos.z / SECTOR_SIZE), (int)(m_vPos.x / SECTOR_SIZE));

	/* 파티에 가입되어 있다면 파티 탈퇴 */
	if (m_bIsPartyState == true)
		process_leave_party_ai();

	/* 해당 플레이어가 등록되어 있는 섹터 내의 유저들에게 접속 종료를 알림 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)m_vPos.x, (int)m_vPos.z);

	// 인접 섹터 순회
	for (auto& s : nearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 오직 유저들에게만 패킷을 전송 (NPC 제외) */
				if (false == CObjMgr::GetInstance()->Is_Player(obj_num)) continue;
				// '나'에게 패킷 전송 X
				if (obj_num == m_sNum) continue;

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

				// 접속한 유저들에게만 접속 종료를 알림
				if (pOther->Get_IsConnected())
				{
					/* 타 유저의 시야 목록 내에 '나'가 있다면 지운다 */
					pOther->v_lock.lock();
					if (0 != pOther->view_list.count(m_sNum))
					{
						pOther->view_list.erase(m_sNum);
						pOther->v_lock.unlock();

						/* 타 유저에게 접속 종료 패킷 전송 */
#ifdef TEST
						cout << obj_num << "님에게" << id << "님 퇴장을 전송" << endl;
#endif					
						send_leave_packet(obj_num, m_sNum);
					}
					else pOther->v_lock.unlock();
				}
			}
		}
	}


	m_bIsConnect	= false;
	m_bIsDead		= false;
	m_vPos			= _vec3(0.f, 0.f, 0.f);
	m_vDir			= _vec3(0.f, 0.f, 0.f);
	m_ID[0]			= 0;
	m_type			= 0;
	m_chStageId		= STAGE_WINTER;
	m_bIsPartyState = false;
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

void CAi::send_leave_party_ai(const int& to_client)
{
	sc_packet_suggest_party p;

	p.size = sizeof(p);
	p.type = SC_PACKET_LEAVE_PARTY;
	p.id = m_sNum;

	send_packet(to_client, &p);
}

DWORD CAi::Release()
{
    return 0;
}
