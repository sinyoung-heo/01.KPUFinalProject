#include "stdafx.h"
#include "Ai.h"
#include "Player.h"

CAi::CAi()
    :m_iLevel(0), m_iHp(0), m_iMaxHp(0), m_fSpd(0.f),
    m_iMp(0), m_iMaxMp(0), m_iMaxAtt(0), m_iMinAtt(0.f),
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
