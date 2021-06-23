#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"

/* ========================패킷 처리========================*/
void process_packet(int id)
{
	/* Server Number 해당 유저 */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	// 패킷 타입
	char p_type = pPlayer->m_packet_start[1];

	switch (p_type)
	{
	case CS_LOGIN:
	{
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(pPlayer->m_packet_start);

		pPlayer->Get_ClientLock().lock();
		strncpy_s(pPlayer->m_ID, p->name, strlen(p->name));
		pPlayer->m_type = p->o_type;
		pPlayer->m_chWeaponType = p->weapon_type;
		pPlayer->Get_ClientLock().unlock();

		// 회원일 경우 DB 검사
		if (p->isMember)
		{
			/* CHECK ID in Database Server */
			if (false == CDBMgr::GetInstance()->Check_ID(id, p->password))
			{
				/* 처음 게임에 접속한 유저라면 회원으로 등록 in Database Server */
				CDBMgr::GetInstance()->Insert_NewPlayer_DB(id, p->password);
			}
		}

		/* 로그인 수락 패킷 전송 */
		send_login_ok(id);

		/* Sector 다시 등록 (접속 시 미리 한 번 하고있음. 완전함을 위해 한 번 더 등록(sector의 Key는 Unique함) */
		CSectorMgr::GetInstance()->Enter_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

		/* 해당 플레이어가 등록되어 있는 섹터 내의 유저들에게 접속을 알림 */
		unordered_set<pair<int, int>> nearSector;
		nearSector.reserve(NEAR_SECTOR);
		CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

		// 인접 섹터 순회
		for (auto& s : nearSector)
		{
			// 인접 섹터 내의 타 유저들이 있는지 검사
			if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
			{
				// 타 유저의 서버 번호 추출
				for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
				{
					// '나'에게 등장 패킷 전송 X
					if (obj_num == id) continue;

					/* 타유저일 경우 처리 */
					if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
					{
						CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

						// 접속한 유저들에게만 접속 종료를 알림
						if (pOther->Get_IsConnected())
						{
							// 시야 내에 없다면 시야 목록에 등록X.
							if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pOther)) continue;
							
							/* 타 유저의 시야에 내가 없을 경우 '나'를 등록시킴 */
							pOther->v_lock.lock();
							if (0 == pOther->view_list.count(id))
							{
								pOther->view_list.insert(id);
								pOther->v_lock.unlock();
								send_enter_packet(obj_num, id);
							}
							else pOther->v_lock.unlock();

							/* '나'의 시야에 타 유저가 없을 경우 -> 타 유저 등록 */
							pPlayer->v_lock.lock();
							if (0 == pPlayer->view_list.count(obj_num))
							{
								pPlayer->view_list.insert(obj_num);
								pPlayer->v_lock.unlock();
								send_enter_packet(id, obj_num);
							}
							else pPlayer->v_lock.unlock();
						}
					}
					/* NPC일 경우 처리 */
					else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
					{
						CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

						// 시야 내에 없다면 시야 목록에 등록X.
						if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC)) continue;
						pPlayer->v_lock.lock();
						pPlayer->view_list.insert(obj_num);
						pPlayer->v_lock.unlock();
						pNPC->send_NPC_enter_packet(id);
					}
					/* MONSTER일 경우 처리*/
					else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
					{
						CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

						// 시야 내에 없다면 시야 목록에 등록X
						if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster)) continue;
						pPlayer->v_lock.lock();
						pPlayer->view_list.insert(obj_num);
						pPlayer->v_lock.unlock();
						
						pMonster->send_Monster_enter_packet(id);
					}
				}
			}
		}
	}
	break;

	case CS_MOVE:
	{
		cs_packet_move* p = reinterpret_cast<cs_packet_move*>(pPlayer->m_packet_start);
		
		pPlayer->move_time = p->move_time;
		pPlayer->m_iAniIdx = p->animIdx;
		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_move(id, vDir, vPos);
	}
	break;

	case CS_MOVE_STOP:
	{
		cs_packet_move_stop* p = reinterpret_cast<cs_packet_move_stop*>(pPlayer->m_packet_start);

		pPlayer->m_iAniIdx = p->animIdx;
		_vec3 vPos = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir = _vec3(p->dirX, p->dirY, p->dirZ);

		process_move_stop(id, vPos, vDir);
	}
	break;

	case CS_STANCE_CHANGE:
	{
		cs_packet_stance_change* p = reinterpret_cast<cs_packet_stance_change*>(pPlayer->m_packet_start);

		pPlayer->m_iAniIdx = p->animIdx;
		pPlayer->m_bIsAttackStance = p->is_stance_attack;
		process_stance_change(id, p->is_stance_attack);
	}
	break;

	case CS_ATTACK:
	{
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(pPlayer->m_packet_start);

		int		iAniIdx    = p->animIdx;
		_vec3	vPos       = _vec3(p->posX, p->posY, p->posZ);
		_vec3	vDir       = _vec3(p->dirX, p->dirY, p->dirZ);
		float	fEndAngleY = p->end_angleY;

		process_attack(id, vDir, vPos, iAniIdx, fEndAngleY); // 스킬 or 기본 공격 애니메이션 인덱스 부여 필요함
	}
	break;

	case CS_ATTACK_STOP:
	{
		cs_packet_attack* p = reinterpret_cast<cs_packet_attack*>(pPlayer->m_packet_start);

		_vec3 vPos  = _vec3(p->posX, p->posY, p->posZ);
		_vec3 vDir  = _vec3(p->dirX, p->dirY, p->dirZ);
	
		process_attack_stop(id, vDir, vPos, p->animIdx);
	}
	break;

	case CS_CHAT:
	{
		cs_packet_chat* p = reinterpret_cast<cs_packet_chat*>(pPlayer->m_packet_start);
		
		process_chat(id, p->message);
	}
	break;

	case CS_LOGOUT:
	{
		process_disconnect(id);
	}
	break;

	case CS_COLLIDE: 
	{
		cs_packet_player_collision* p = reinterpret_cast<cs_packet_player_collision*>(pPlayer->m_packet_start);
		process_collide(id, p->col_id, p->damage);
	}
	break;

	case CS_COLLIDE_MONSTER:
	{
		cs_packet_player_collision* p = reinterpret_cast<cs_packet_player_collision*>(pPlayer->m_packet_start);

		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", p->col_id));
		if (nullptr == pMonster) return;

		pMonster->Hurt_Monster(id, p->damage, p->affect);
	}
	break;

	case CS_STAGE_CHANGE:
	{
		cs_packet_stage_change* p = reinterpret_cast<cs_packet_stage_change*>(pPlayer->m_packet_start);
		process_stage_change(id, p->stage_id);
	}
	break;

	case CS_SUGGEST_PARTY:
	{
		cs_packet_suggest_party* p = reinterpret_cast<cs_packet_suggest_party*>(pPlayer->m_packet_start);
		process_suggest_party(id, p->member_id);
	}
	break;

	case CS_RESPOND_PARTY:
	{
		cs_packet_respond_party* p = reinterpret_cast<cs_packet_respond_party*>(pPlayer->m_packet_start);
		process_respond_party(p->result, p->suggester_id, id);
	}
	break;

	case CS_JOIN_PARTY:
	{
		cs_packet_suggest_party* p = reinterpret_cast<cs_packet_suggest_party*>(pPlayer->m_packet_start);
		process_join_party(id, p->member_id);
	}
	break;

	case CS_DECIDE_PARTY:
	{
		cs_packet_respond_party* p = reinterpret_cast<cs_packet_respond_party*>(pPlayer->m_packet_start);
		process_decide_party(p->result, p->suggester_id, id);
	}
	break;

	case CS_LEAVE_PARTY:
	{
		cs_packet_suggest_party* p = reinterpret_cast<cs_packet_suggest_party*>(pPlayer->m_packet_start);
		process_leave_party(id);
	}
	break;

	case CS_ADD_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_add_item(id, p->itemType, p->itemName);
	}
	break;

	case CS_DELETE_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_delete_item(id, p->itemType, p->itemName);
	}
	break;

	case CS_EQUIP_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_equip_item(id, p->itemType, p->itemName);
	}
	break;

	case CS_UNEQUIP_ITEM:
	{
		cs_packet_manage_inventory* p = reinterpret_cast<cs_packet_manage_inventory*>(pPlayer->m_packet_start);
		process_unequip_item(id, p->itemType, p->itemName);
	}
	break;

	}
}
/* ========================패킷 재조립========================*/
void process_recv(int id, DWORD iosize)
{
	/* Server Number 해당 유저 */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;
	
	// m_packet_start		: 처리할 데이터 버퍼 및 위치 (= iocp_buffer)
	// m_packet_start[0]	: 처리할 패킷의 크기
	unsigned char p_size = pPlayer->m_packet_start[0];
	
	/* 다음 패킷을 받을 링버퍼 위치 설정 */
	// m_recv_start			: 수신한 패킷의 시작 위치
	unsigned char* next_recv_ptr = pPlayer->m_recv_start + iosize;

	// 처리할 패킷 크기보다 남아있는 공간이 적당할 경우 패킷 처리 시작
	while (p_size <= next_recv_ptr - pPlayer->m_packet_start)
	{
		// 패킷 처리 (= 패킷 만들기)
		process_packet(id);
		// 패킷 처리 후 유저의 데이터 처리 버퍼 시작 위치 변경
		pPlayer->m_packet_start += p_size;
		
		// recv한 데이터를 처리를 했는데 아직 처리하지 못한 데이터를 가지고 있을 수도 있다.
		// 처리해야 할 패킷 크기 갱신
		if (pPlayer->m_packet_start < next_recv_ptr)
			p_size = pPlayer->m_packet_start[0];
		else
			break;
	} 

	// 처리하지 못한(남아 있는) 데이터의 시작 위치 
	long long left_data = next_recv_ptr - pPlayer->m_packet_start;

	// Recv 처리 끝 -> 다시 Recv를 해야 한다.
	// 다시 Recv를 하기 전에 링 버퍼의 여유 공간을 확인 후 다 사용했을 경우 초기화
	// 남아있는 공간이 MIN_BUUFER 보다 작으면 남은 데이터를 링버퍼 맨 앞으로 옮겨준다. 
	if ((MAX_BUFFER - (next_recv_ptr - pPlayer->m_recv_over.iocp_buf)) < MIN_BUFFER)
	{
		// 남은 데이터 버퍼를 iocp_buf에 복사
		memcpy(pPlayer->m_recv_over.iocp_buf, pPlayer->m_packet_start, left_data);
		// 처리할 데이터 버퍼 갱신
		pPlayer->m_packet_start = pPlayer->m_recv_over.iocp_buf;
		// 다음 패킷을 받을 버퍼의 시작위치 갱신
		next_recv_ptr = pPlayer->m_packet_start + left_data;
	}

	// 링 버퍼 공간 갱신 여부 확인 후 다음 Recv 준비하기
	DWORD recv_flag = 0;
	pPlayer->m_recv_start = next_recv_ptr;
	pPlayer->m_recv_over.wsa_buf.buf = reinterpret_cast<CHAR*>(next_recv_ptr);
	pPlayer->m_recv_over.wsa_buf.len = MAX_BUFFER - static_cast<int>(next_recv_ptr - pPlayer->m_recv_over.iocp_buf);

	pPlayer->Get_ClientLock().lock();
	if (pPlayer->Get_IsConnected())
		WSARecv(pPlayer->m_sock, &pPlayer->m_recv_over.wsa_buf, 1, NULL, &recv_flag, &pPlayer->m_recv_over.wsa_over, NULL);
	pPlayer->Get_ClientLock().unlock();
}

/*===========================================PLAYER=====================================================*/
void send_packet(int id, void* p)
{
	unsigned char* packet = reinterpret_cast<unsigned char*>(p);

	OVER_EX* send_over = new OVER_EX;

	memcpy(send_over->iocp_buf, packet, packet[0]);
	send_over->op_mode = OP_MODE_SEND;
	send_over->wsa_buf.buf = reinterpret_cast<CHAR*>(send_over->iocp_buf);
	send_over->wsa_buf.len = packet[0];
	ZeroMemory(&send_over->wsa_over, sizeof(send_over->wsa_over));

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	pPlayer->Get_ClientLock().lock();
	if (pPlayer->Get_IsConnected())
		WSASend(pPlayer->m_sock, &send_over->wsa_buf, 1, NULL, 0, &send_over->wsa_over, NULL);
	pPlayer->Get_ClientLock().unlock();
}

void send_login_ok(int id)
{
	sc_packet_login_ok p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_LOGIN_OK;
	p.id = id;

	p.o_type		= pPlayer->m_type;
	p.weaponType	= pPlayer->m_chWeaponType;
	p.naviType		= pPlayer->m_chStageId;

	p.level			= pPlayer->m_iLevel;
	p.hp			= pPlayer->m_iHp;
	p.maxHp			= pPlayer->m_iMaxHp;
	p.mp			= pPlayer->m_iMp;
	p.maxMp			= pPlayer->m_iMaxMp;
	p.exp			= pPlayer->m_iExp;
	p.maxExp		= pPlayer->m_iMaxExp;
	p.min_att		= pPlayer->m_iMinAtt;
	p.max_att		= pPlayer->m_iMaxAtt;
	p.spd			= pPlayer->m_fSpd;

	p.posX			= pPlayer->m_vPos.x;
	p.posY			= pPlayer->m_vPos.y;
	p.posZ			= pPlayer->m_vPos.z;

	send_packet(id, &p);
}

void send_enter_packet(int to_client, int new_id)
{
	sc_packet_enter p;

	CPlayer* pNewPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", new_id));

	p.size	= sizeof(p);
	p.type	= SC_PACKET_ENTER;
	p.id	= new_id;

	pNewPlayer->Get_ClientLock().lock();
	strncpy_s(p.name, pNewPlayer->m_ID, strlen(pNewPlayer->m_ID));

	pNewPlayer->Get_ClientLock().unlock();
	p.o_type			= pNewPlayer->m_type;
	p.weaponType		= pNewPlayer->m_chWeaponType;
	p.stageID			= pNewPlayer->m_chStageId;
	p.is_stance_attack	= pNewPlayer->m_bIsAttackStance;
	p.is_party_state	= pNewPlayer->m_bIsPartyState;

	p.posX				= pNewPlayer->m_vPos.x;
	p.posY				= pNewPlayer->m_vPos.y;
	p.posZ				= pNewPlayer->m_vPos.z;

	p.dirX				= pNewPlayer->m_vDir.x;
	p.dirY				= pNewPlayer->m_vDir.y;
	p.dirZ				= pNewPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void send_leave_packet(int to_client, int leave_id)
{
	sc_packet_leave p;

	p.size = sizeof(p);
	p.type = SC_PACKET_LEAVE;
	p.id = leave_id;
	
	send_packet(to_client, &p);
}

void send_move_packet(int to_client, int id)
{
	sc_packet_move p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE;
	p.id = id;

	p.spd = pPlayer->m_fSpd;
	p.animIdx = pPlayer->m_iAniIdx;
	p.move_time = pPlayer->move_time;

	p.posX = pPlayer->m_vTempPos.x;
	p.posY = pPlayer->m_vTempPos.y;
	p.posZ = pPlayer->m_vTempPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void send_move_stop_packet(int to_client, int id)
{
	sc_packet_move p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_MOVE_STOP;
	p.id = id;

	p.spd = pPlayer->m_fSpd;
	p.animIdx = pPlayer->m_iAniIdx;
	p.move_time = pPlayer->move_time;

	p.posX = pPlayer->m_vPos.x;
	p.posY = pPlayer->m_vPos.y;
	p.posZ = pPlayer->m_vPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void send_attack_packet(int to_client, int id, int animIdx, float end_angleY)
{
	sc_packet_attack p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_ATTACK;

	p.id = id;
	p.o_type = pPlayer->m_type;

	p.posX = pPlayer->m_vTempPos.x;
	p.posY = pPlayer->m_vTempPos.y;
	p.posZ = pPlayer->m_vTempPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	p.animIdx = animIdx;
	p.end_angleY = end_angleY;

	send_packet(to_client, &p);
}

void send_attack_stop_packet(int to_client, int id, int animIdx)
{
	sc_packet_attack p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	p.size = sizeof(p);
	p.type = SC_PACKET_ATTACK_STOP;

	p.id = id;
	p.o_type = pPlayer->m_type;
	p.animIdx = animIdx;

	p.posX = pPlayer->m_vPos.x;
	p.posY = pPlayer->m_vPos.y;
	p.posZ = pPlayer->m_vPos.z;

	p.dirX = pPlayer->m_vDir.x;
	p.dirY = pPlayer->m_vDir.y;
	p.dirZ = pPlayer->m_vDir.z;

	send_packet(to_client, &p);
}

void send_player_stat(int to_client, int id)
{
	sc_packet_stat_change p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	p.size		= sizeof(p);
	p.type		= SC_PACKET_STAT_CHANGE;

	p.id		= id;
	p.hp		= pPlayer->m_iHp;
	p.maxHp		= pPlayer->m_iMaxHp;
	p.mp		= pPlayer->m_iMp;
	p.maxMp		= pPlayer->m_iMaxMp;
	p.exp		= pPlayer->m_iExp;
	p.maxExp	= pPlayer->m_iMaxExp;
	p.lev		= pPlayer->m_iLevel;
	p.maxAtt	= pPlayer->m_iMaxAtt;
	p.minAtt	= pPlayer->m_iMinAtt;

	send_packet(to_client, &p);
}

void send_player_stance_change(int to_client, int id, const bool& st)
{
	sc_packet_stance_change p;

	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	p.size             = sizeof(p);
	p.type             = SC_PACKET_STANCE_CHANGE;
	p.id               = id;
	p.animIdx          = pPlayer->m_iAniIdx;
	p.o_type           = pPlayer->m_type;
	p.is_stance_attack = st;

	send_packet(to_client, &p);
}

void send_player_stage_change(int to_client, int id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	sc_packet_stage_change p;

	p.size     = sizeof(p);
	p.type     = SC_PACKET_STAGE_CHANGE;
	p.id	   = id;

	p.stage_id = pPlayer->m_chStageId;
	p.posX     = pPlayer->m_vPos.x;
	p.posY     = pPlayer->m_vPos.y;
	p.posZ     = pPlayer->m_vPos.z;

	send_packet(to_client, &p);
}

void send_suggest_party(int to_client, int id)
{
	sc_packet_suggest_party p;

	p.size = sizeof(p);
	p.type = SC_PACKET_SUGGEST_PARTY;
	p.id = id;

	send_packet(to_client, &p);
}

void send_enter_party(int to_client, int id, const int& hp, const int& maxHp, const int& mp, const int& maxMp, const char* ID, const char& job)
{
	sc_packet_enter_party p;

	p.size = sizeof(p);
	p.type = SC_PACKET_ENTER_PARTY;
	p.id = id;

	strncpy_s(p.name, ID, strlen(ID));
	p.o_type = job;
	p.hp = hp;
	p.maxHp = maxHp;
	p.mp = mp;
	p.maxMp = maxMp;

	send_packet(to_client, &p);
}

void send_reject_party(int to_client, int id)
{
	sc_packet_chat p;

	p.size = sizeof(p);
	p.type = SC_PACKET_REJECT_PARTY;
	p.id = id;

	strncpy_s(p.name, "0", strlen("0"));
	strncpy_s(p.message, "파티 참여를 거절하였습니다.", strlen("파티 참여를 거절하였습니다."));
	
	send_packet(to_client, &p);
}

void send_join_party(int to_client, int id)
{
	sc_packet_suggest_party p;

	p.size = sizeof(p);
	p.type = SC_PACKET_JOIN_PARTY;
	p.id = id;

	send_packet(to_client, &p);
}

void send_leave_party(int to_client, int id)
{
	sc_packet_suggest_party p;

	p.size = sizeof(p);
	p.type = SC_PACKET_LEAVE_PARTY;
	p.id = id;

	send_packet(to_client, &p);
}

void send_update_party(const int& to_client, const int& id, const int& hp, const int& maxHp, const int& mp, const int& maxMp)
{
	sc_packet_update_party p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_UPDATE_PARTY;
	p.id	= id;

	p.hp	= hp;
	p.maxHp = maxHp;
	p.mp	= mp;
	p.maxMp = maxMp;

	send_packet(to_client, &p);
}

void send_chat(const int& to_client, const int& id, const char* name, const char* buffer, const int len)
{
	sc_packet_chat p;

	p.size	= sizeof(p);
	p.type	= SC_PACKET_CHAT;
	p.id	= id;

	strncpy_s(p.name, name, strlen(name));
	strncpy_s(p.message, buffer, len);
	p.messageLen = len;

	send_packet(to_client, &p);
}

void send_update_inventory(const int& id, const char& chItemType, const char& chName, const int& count)
{
	sc_packet_update_inventory p;

	p.size		= sizeof(p);
	p.type		= SC_PACKET_UPDATE_INVENTORY;

	p.itemType	= chItemType;
	p.itemName	= chName;
	p.count		= count;

	send_packet(id, &p);
}

void send_update_equipment(const int& to_client, const char& chItemType, const char& chName)
{
	sc_packet_update_equipment p;

	p.size = sizeof(p);
	p.type = SC_PACKET_UPDATE_EQUIPMENT;

	p.equipType = chItemType;
	p.itemName = chName;

	send_packet(to_client, &p);
}

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* 해당 플레이어의 원래 위치값 & 변경된 위치값 */
	float ori_x, ori_y, ori_z;	
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* 해당 플레이어의 방향벡터와 위치벡터 */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;
	
	/* 해당 플레이어의 원래 시야 목록 */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* 해당 플레이어의 미래 위치 좌표 산출 -> 미래 위치좌표는 임시 변수에 저장 */
	_vec2 coll_pos = _vec2(0.f);

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos, pPlayer->m_chStageId))
	{
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;
	}

	send_move_packet(id, id);

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* 플레이어 주변의 섹터를 검색 -> 인접한 섹터 내의 객체에게 좌표 전송 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// 해당 플레이어의 갱신된 시야 목록
	unordered_set<int> new_viewlist;

	// 인접 섹터 순회
	for (auto& s : nearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));
					
					// 접속한 유저만 시야 목록에 등록한다.
					if (!pOther->Get_IsConnected()) continue;
					
					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);			
				}
				/* NPC일 경우 처리 */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER일 경우 처리*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* 새로운 시야 목록 내의 객체 처리 */
	for (int server_num : new_viewlist)
	{
		// 플레이어 시야 목록에 새로 들어온 객체 처리 (이전 시야 목록에 없다면)
		if (0 == old_viewlist.count(server_num))
		{
			// 플레이어 시야 목록에 추가
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// 새로운 타유저의 시야 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// 플레이어('나')에게 새로운 유저 등장 패킷 전송
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
				else
				{
					pOther->v_lock.unlock();
					send_move_packet(server_num, id);
				}
			}
			// 새로 시야에 들어온 NPC일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// 플레이어('나')에게 NPC등장 패킷 전송
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// 새로 시야에 들어온 MONSTER일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// 플레이어('나')에게 Monster 등장 패킷 전송
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// 플레이어 시야 목록에 계속 있는 객체 처리
		else
		{
			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 계속 있는 경우
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_move_packet(server_num, id);
				}
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // 상대방에게 나의 스탯 정보 갱신 .... 나중에
	}

	/* 이전 시야 목록에서 사라진 객체 처리 */
	for (int s_num : old_viewlist)
	{
		// 갱신된 시야 목록에 없는 객체일 경우
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();

				// 타 유저의 시야에 '나'가 있다면 삭제
				if (0 != pOther->view_list.count(id))
				{
					pOther->view_list.erase(id);
					pOther->v_lock.unlock();
					send_leave_packet(s_num, id);
				}
				else pOther->v_lock.unlock();
			}
		}
	}

#ifdef TEST
	/* 플레이어 시야 내에 있는 객체 출력 */
	for (int server_obj : new_viewlist)
		cout << "move" << server_obj << "시야 내에 존재합니다." << endl;
#endif
}

void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* 해당 플레이어의 원래 위치값 & 변경된 위치값 */
	float ori_x, ori_y, ori_z;

	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* 해당 플레이어의 원래 시야 목록 */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* 해당 플레이어로부터 받은 최종 위치값 저장 */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;
	
	send_move_stop_packet(id, id);

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* 플레이어 주변의 섹터를 검색 -> 인접한 섹터 내의 객체에게 좌표 전송 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// 해당 플레이어의 갱신된 시야 목록
	unordered_set<int> new_viewlist;

	// 인접 섹터 순회
	for (auto& s : nearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pOther->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);
				}
				/* NPC일 경우 처리 */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER일 경우 처리*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* 새로운 시야 목록 내의 객체 처리 */
	for (int server_num : new_viewlist)
	{
		// 플레이어 시야 목록에 새로 들어온 객체 처리 (이전 시야 목록에 없다면)
		if (0 == old_viewlist.count(server_num))
		{
			// 플레이어 시야 목록에 추가
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// 새로운 타유저의 시야 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// 플레이어('나')에게 새로운 유저 등장 패킷 전송
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
				else
				{
					pOther->v_lock.unlock();
					send_move_stop_packet(server_num, id);
				}
			}
			// 새로 시야에 들어온 NPC일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// 플레이어('나')에게 NPC등장 패킷 전송
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// 새로 시야에 들어온 MONSTER일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// 플레이어('나')에게 Monster 등장 패킷 전송
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// 플레이어 시야 목록에 계속 있는 객체 처리
		else
		{
			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 계속 있는 경우
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_move_stop_packet(server_num, id);
				}
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // 상대방에게 나의 스탯 정보 갱신 .... 나중에
	}

	/* 이전 시야 목록에서 사라진 객체 처리 */
	for (int s_num : old_viewlist)
	{
		// 갱신된 시야 목록에 없는 객체일 경우
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();

				// 타 유저의 시야에 '나'가 있다면 삭제
				if (0 != pOther->view_list.count(id))
				{
					pOther->view_list.erase(id);
					pOther->v_lock.unlock();
					send_leave_packet(s_num, id);
				}
				else pOther->v_lock.unlock();
			}
		}
	}

#ifdef TEST
	/* 플레이어 시야 내에 있는 객체 출력 */
	for (int server_obj : new_viewlist)
		cout << "move_stop" << server_obj << "시야 내에 존재합니다." << endl;
#endif
}

void process_collide(int id, int colID, int damage)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	/* COLLIDE PLAYER - NPC */
	if (colID >= NPC_NUM_START && colID < MON_NUM_START){}

	/* COLLIDE PLAYER - MONSTER */
	else if (colID >= MON_NUM_START)
	{
		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", colID));
		if (nullptr == pMonster) return;

		/* Decrease Player HP */
		if (pPlayer->m_iHp > 0)
			pPlayer->m_iHp -= damage;
		else
		{
			/* Player Dead */
			pPlayer->m_iHp = ZERO_HP;
			pPlayer->Set_IsDead(true);
		}

		/* 해당 플레이어의 시야 목록 */
		pPlayer->v_lock.lock();
		unordered_set<int> viewlist = pPlayer->view_list;
		pPlayer->v_lock.unlock();

		/* 해당 유저에게 바뀐 stat 전송 */
		send_player_stat(id, id);

		/* 해당 유저가 파티에 가입되어 있는 상태일 경우 파티원에게 전송 */
		if (pPlayer->m_bIsPartyState)
		{
			for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pPlayer->m_iPartyNumber))
			{
				if (p == id) continue;
				send_update_party(p, id, pPlayer->m_iHp, pPlayer->m_iMaxHp, pPlayer->m_iMp, pPlayer->m_iMaxMp);
			}
		}
	}
}

void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* 해당 플레이어의 원래 위치값 & 변경된 위치값 */
	float ori_x, ori_y, ori_z;
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* 해당 플레이어의 방향벡터와 위치벡터 */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;

	/* 해당 플레이어의 원래 시야 목록 */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* 해당 플레이어의 미래 위치 좌표 산출 -> 미래 위치좌표는 임시 변수에 저장 */
	_vec2 coll_pos = _vec2(0.f);

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos, pPlayer->m_chStageId))
	{
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;
	}

	send_attack_packet(id, id, aniIdx, end_angleY);

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* 플레이어 주변의 섹터를 검색 -> 인접한 섹터 내의 객체에게 좌표 전송 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// 해당 플레이어의 갱신된 시야 목록
	unordered_set<int> new_viewlist;

	// 인접 섹터 순회
	for (auto& s : nearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pOther->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);
				}
				/* NPC일 경우 처리 */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER일 경우 처리*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* 새로운 시야 목록 내의 객체 처리 */
	for (int server_num : new_viewlist)
	{
		// 플레이어 시야 목록에 새로 들어온 객체 처리 (이전 시야 목록에 없다면)
		if (0 == old_viewlist.count(server_num))
		{
			// 플레이어 시야 목록에 추가
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// 새로운 타유저의 시야 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// 플레이어('나')에게 새로운 유저 등장 패킷 전송
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
				else
				{
					pOther->v_lock.unlock();
					send_attack_packet(server_num, id,aniIdx, end_angleY);
				}
			}
			// 새로 시야에 들어온 NPC일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// 플레이어('나')에게 NPC등장 패킷 전송
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// 새로 시야에 들어온 MONSTER일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// 플레이어('나')에게 Monster 등장 패킷 전송
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// 플레이어 시야 목록에 계속 있는 객체 처리
		else
		{
			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 계속 있는 경우
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_attack_packet(server_num, id, aniIdx, end_angleY);
				}
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // 상대방에게 나의 스탯 정보 갱신 .... 나중에
	}

	/* 이전 시야 목록에서 사라진 객체 처리 */
	for (int s_num : old_viewlist)
	{
		// 갱신된 시야 목록에 없는 객체일 경우
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();

				// 타 유저의 시야에 '나'가 있다면 삭제
				if (0 != pOther->view_list.count(id))
				{
					pOther->view_list.erase(id);
					pOther->v_lock.unlock();
					send_leave_packet(s_num, id);
				}
				else pOther->v_lock.unlock();
			}
		}
	}

#ifdef TEST
	/* 플레이어 시야 내에 있는 객체 출력 */
	for (int server_obj : new_viewlist)
		cout << "attack" << server_obj << "시야 내에 존재합니다." << endl;
#endif
}

void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* 해당 플레이어의 원래 위치값 & 변경된 위치값 */
	float ori_x, ori_y, ori_z;

	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* 해당 플레이어의 원래 시야 목록 */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* 해당 플레이어로부터 받은 최종 위치값 저장 */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;

	send_attack_stop_packet(id, id, aniIdx);

	/* 변경된 좌표로 섹터 갱신 */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* 플레이어 주변의 섹터를 검색 -> 인접한 섹터 내의 객체에게 좌표 전송 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// 해당 플레이어의 갱신된 시야 목록
	unordered_set<int> new_viewlist;

	// 인접 섹터 순회
	for (auto& s : nearSector)
	{
		// 인접 섹터 내의 타 유저들이 있는지 검사
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// 타 유저의 서버 번호 추출
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* 타유저일 경우 처리 */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// 접속한 유저만 시야 목록에 등록한다.
					if (!pOther->Get_IsConnected()) continue;

					// 시야 내에 있다면 시야 목록에 등록한다.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);
				}
				/* NPC일 경우 처리 */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER일 경우 처리*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));
					if (pMonster->Get_Dead() == true) continue;

					// 시야 내에 없다면 시야 목록에 등록X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* 새로운 시야 목록 내의 객체 처리 */
	for (int server_num : new_viewlist)
	{
		// 플레이어 시야 목록에 새로 들어온 객체 처리 (이전 시야 목록에 없다면)
		if (0 == old_viewlist.count(server_num))
		{
			// 플레이어 시야 목록에 추가
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// 새로운 타유저의 시야 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// 플레이어('나')에게 새로운 유저 등장 패킷 전송
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				if (0 == pOther->view_list.count(id))
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
				else
				{
					pOther->v_lock.unlock();
					send_attack_stop_packet(server_num, id, aniIdx);
				}
			}
			// 새로 시야에 들어온 NPC일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// 플레이어('나')에게 NPC등장 패킷 전송
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// 새로 시야에 들어온 MONSTER일 경우 처리
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// 플레이어('나')에게 Monster 등장 패킷 전송
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// 플레이어 시야 목록에 계속 있는 객체 처리
		else
		{
			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();
				// 타 유저의 시야 목록에 '나'가 계속 있는 경우
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_attack_stop_packet(server_num, id, aniIdx);
				}
				// 타 유저의 시야 목록에 '나'가 새로 들어온 경우
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // 상대방에게 나의 스탯 정보 갱신 .... 나중에
	}

	/* 이전 시야 목록에서 사라진 객체 처리 */
	for (int s_num : old_viewlist)
	{
		// 갱신된 시야 목록에 없는 객체일 경우
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// 타 유저 처리
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// 타 유저의 시야 목록 처리
				pOther->v_lock.lock();

				// 타 유저의 시야에 '나'가 있다면 삭제
				if (0 != pOther->view_list.count(id))
				{
					pOther->view_list.erase(id);
					pOther->v_lock.unlock();
					send_leave_packet(s_num, id);
				}
				else pOther->v_lock.unlock();
			}
		}
	}

#ifdef TEST
	/* 플레이어 시야 내에 있는 객체 출력 */
	for (int server_obj : new_viewlist)
		cout << "attck_stop" << server_obj << "시야 내에 존재합니다." << endl;
#endif
}

void process_stance_change(int id, const bool& stance)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* 해당 플레이어의 원래 시야 목록 */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	for (int server_num : old_viewlist)
	{
		if (id == server_num) continue;
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pOther == nullptr) continue;
			if (!pOther->m_bIsConnect) continue;

			send_player_stance_change(server_num, id, stance);
		}
	}
}

void process_stage_change(int id, const char& stage_id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	// Set StageID
	pPlayer->m_chStageId = stage_id;

	// Set Position
	if (STAGE_VELIKA == stage_id)
		pPlayer->m_vPos = _vec3(STAGE_VELIKA_X, 0.0f, STAGE_VELIKA_Z);

	else if (STAGE_BEACH == stage_id)
		pPlayer->m_vPos = _vec3(STAGE_BEACH_X, 0.0f, STAGE_BEACH_Z);

	else if (STAGE_WINTER == stage_id)
		pPlayer->m_vPos = _vec3(STAGE_WINTER_X, 0.0f, STAGE_WINTER_Z);

	// Send Packet
	send_player_stage_change(id, id);

	/* 해당 플레이어의 원래 시야 목록 */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	for (int server_num : old_viewlist)
	{
		if (id == server_num) continue;
		if (true == CObjMgr::GetInstance()->Is_Player(server_num))
		{
			CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));
			if (pOther == nullptr) continue;
			if (!pOther->m_bIsConnect) continue;

			send_player_stage_change(server_num, id);
		}
	}
}

void process_suggest_party(const int& suggester_id, const int& others_id)
{
	if (true == CObjMgr::GetInstance()->Is_Player(others_id))
	{
		CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", others_id));
		if (pOther == nullptr) return;
		if (!pOther->m_bIsConnect) return;
		if (pOther->m_bIsPartyState == true) return;

		send_suggest_party(others_id, suggester_id);
	}
}

void process_respond_party(const bool& result, const int& suggester_id, const int& responder_id)
{
	// 제안 거절
	if (result == false)
	{
		// 거절 메시지 전송
		send_reject_party(suggester_id, responder_id);
	}
	// 제안 수락
	else
	{
		// 1. 파티초대자가 참여된 파티가 없을 경우
		CPlayer* pSuggester = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", suggester_id));
		if (pSuggester == nullptr) return;
		if (!pSuggester->m_bIsConnect) return;

		CPlayer* pResponder = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", responder_id));
		if (pResponder == nullptr) return;
		if (!pResponder->m_bIsConnect) return;

		if (pSuggester->m_bIsPartyState == false)
		{
			// 1-1. 새로운 파티 생성
			CObjMgr::GetInstance()->Create_Party(&pSuggester->m_iPartyNumber, suggester_id);

			// 1-2. 새로운 파티 생성 후 멤버 초대
			CObjMgr::GetInstance()->Add_PartyMember(pSuggester->m_iPartyNumber, &pResponder->m_iPartyNumber, responder_id);

			// 1-3. 초대자 파티 참여 상태로 변경
			pSuggester->m_bIsPartyState = true;

			// 1-4. 새로운 멤버 파티 참여 상태로 변경
			pResponder->m_bIsPartyState = true;
		}
		// 2. 파티초대자가 참여된 파티가 있을 경우
		else
		{
			// 2-1. 새로운 멤버 초대
			CObjMgr::GetInstance()->Add_PartyMember(pSuggester->m_iPartyNumber, &pResponder->m_iPartyNumber, responder_id);
			
			// 2-2. 새로운 멤버 파티 참여 상태로 변경
			pResponder->m_bIsPartyState = true;
		}

		// 3. 파티구성원들에게 새로운 파티멤버 정보 전송
		for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pSuggester->m_iPartyNumber))
		{
			if (p != responder_id)
			{
				CPlayer* pMember = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", p));
				if (pMember == nullptr) return;
				if (!pMember->m_bIsConnect) return;

				// 새로운 멤버 정보 -> 기존 구성원
				send_enter_party(p, responder_id, pResponder->m_iHp, pResponder->m_iMaxHp, pResponder->m_iMp, pResponder->m_iMaxMp, pResponder->m_ID, pResponder->m_type);
				// 기존 구성원 정보 -> 새로운 멤버
				send_enter_party(responder_id, p, pMember->m_iHp, pMember->m_iMaxHp, pMember->m_iMp, pMember->m_iMaxMp, pMember->m_ID, pMember->m_type);
			}
		}
	}
}

void process_join_party(const int& joinner_id, const int& others_id)
{
	if (true == CObjMgr::GetInstance()->Is_Player(others_id))
	{
		CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", others_id));
		if (pOther == nullptr) return;
		if (!pOther->m_bIsConnect) return;
		if (!pOther->m_bIsPartyState) return;

		send_join_party(others_id, joinner_id);
	}
}

void process_decide_party(const bool& result, const int& joinner_id, const int& responder_id)
{
	// 참여 신청 거절
	if (result == false)
	{
		// 거절 메시지 전송
		send_reject_party(joinner_id, responder_id);
	}
	// 참여 신청 수락
	else
	{
		CPlayer* pJoinner = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", joinner_id));
		if (pJoinner == nullptr) return;
		if (!pJoinner->m_bIsConnect) return;

		CPlayer* pResponder = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", responder_id));
		if (pResponder == nullptr) return;
		if (!pResponder->m_bIsConnect) return;

		// 1-1. 새로운 멤버 참여 수락 -> 파티원 목록에 저장
		CObjMgr::GetInstance()->Add_PartyMember(pResponder->m_iPartyNumber, &pJoinner->m_iPartyNumber, joinner_id);

		// 1-2. 새로운 멤버 파티 참여 상태로 변경
		pJoinner->m_bIsPartyState = true;

		// 2. 파티구성원들에게 새로운 파티멤버 정보 전송
		for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pResponder->m_iPartyNumber))
		{
			if (p != joinner_id)
			{
				CPlayer* pMember = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", p));
				if (pMember == nullptr) return;
				if (!pMember->m_bIsConnect) return;

				// 새로운 멤버 정보 -> 기존 구성원
				send_enter_party(p, joinner_id, pJoinner->m_iHp, pJoinner->m_iMaxHp, pJoinner->m_iMp, pJoinner->m_iMaxMp, pJoinner->m_ID, pJoinner->m_type);
				// 기존 구성원 정보 -> 새로운 멤버
				send_enter_party(joinner_id, p, pMember->m_iHp, pMember->m_iMaxHp, pMember->m_iMp, pMember->m_iMaxMp, pMember->m_ID, pMember->m_type);
			}
		}
	}
}

void process_leave_party(const int& id)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;
	if (!pUser->m_bIsPartyState) return;
	
	// 파티 구성원들에게 파티 탈퇴를 알림
	for (auto& p : *CObjMgr::GetInstance()->Get_PARTYLIST(pUser->m_iPartyNumber))
	{
		if (p != id)
		{
			// 탈퇴 멤버 정보 -> 기존 구성원
			send_leave_party(p, id);
		}	
	}

	// 해당 유저의 파티 정보 초기화
	CObjMgr::GetInstance()->Leave_Party(&pUser->m_iPartyNumber, id);
	pUser->m_bIsPartyState = false;
}

void process_disconnect(const int& id)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr || pPlayer->m_bIsConnect == false) return;

	/* sector에서 해당 플레이어 지우기 */
	CSectorMgr::GetInstance()->Leave_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

	/* 파티에 가입되어 있다면 파티 탈퇴 */
	if (pPlayer->m_bIsPartyState == true)
		process_leave_party(id);

	/* 해당 플레이어가 등록되어 있는 섹터 내의 유저들에게 접속 종료를 알림 */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(NEAR_SECTOR);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

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
				if (obj_num == id) continue;

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

				// 접속한 유저들에게만 접속 종료를 알림
				if (pOther->Get_IsConnected())
				{
					/* 타 유저의 시야 목록 내에 '나'가 있다면 지운다 */
					pOther->v_lock.lock();
					if (0 != pOther->view_list.count(id))
					{
						pOther->view_list.erase(id);
						pOther->v_lock.unlock();

						/* 타 유저에게 접속 종료 패킷 전송 */
#ifdef TEST
						cout << obj_num << "님에게" << id << "님 퇴장을 전송" << endl;
#endif					
						send_leave_packet(obj_num, id);
					}
					else pOther->v_lock.unlock();
				}
			}
		}
	}

	pPlayer->Get_ClientLock().lock();

#ifndef DUMMY
	// DB 정보 저장 후 종료
	CDBMgr::GetInstance()->Update_stat_DB(id);
#endif // !DUMMY

	pPlayer->Set_IsConnected(false);
	closesocket(pPlayer->m_sock);
	pPlayer->m_sock				= 0;
	pPlayer->m_vPos				= _vec3(0.f, 0.f, 0.f);
	pPlayer->m_vDir				= _vec3(0.f, 0.f, 0.f);
	pPlayer->m_ID[0]			= 0;
	pPlayer->m_type				= 0;
	pPlayer->m_chStageId		= STAGE_VELIKA;
	pPlayer->m_bIsPartyState	= false;
	pPlayer->m_iPartyNumber		= INIT_PARTY_NUMBER;
	pPlayer->m_iMoney			= INIT_MONEY;
	pPlayer->view_list.clear();
	pPlayer->Release_Inventory();
	pPlayer->Get_ClientLock().unlock();

	CObjPoolMgr::GetInstance()->return_Object(L"PLAYER", pPlayer);
	CObjMgr::GetInstance()->Delete_GameObject(L"PLAYER", pPlayer);
}

void process_chat(const int& id, const char* buffer)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pPlayer == nullptr) return;

	auto iter_begin = CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->begin();
	auto iter_end = CObjMgr::GetInstance()->Get_OBJLIST(L"PLAYER")->end();

	int tempSize = 0;
	Check_Korean(buffer, &tempSize);

	for (iter_begin; iter_begin != iter_end; ++iter_begin)
	{
		send_chat(iter_begin->second->m_sNum, id, pPlayer->m_ID, buffer, tempSize);
	}
}

void process_add_item(const int& id, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	if (pUser->Add_Item(chName, static_cast<ITEM>(chItemType)))
	{
		send_update_inventory(id, chItemType, chName, pUser->Get_ItemCount(chName, static_cast<ITEM>(chItemType)));
	}
}

void process_delete_item(const int& id, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	if (pUser->Delete_Item(chName, static_cast<ITEM>(chItemType)))
	{
		send_update_inventory(id, chItemType, chName, pUser->Get_ItemCount(chName, static_cast<ITEM>(chItemType)));
	}
}

void process_equip_item(const int& id, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	pUser->Equip_Item(chName, chItemType);

	send_update_equipment(id, chItemType, pUser->Get_Equipment(chItemType));
}

void process_unequip_item(const int& id, const char& chItemType, const char& chName)
{
	CPlayer* pUser = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));
	if (pUser == nullptr) return;
	if (!pUser->m_bIsConnect) return;

	pUser->Unequip_Item(chName, chItemType);

	send_update_equipment(id, chItemType, pUser->Get_Equipment(chItemType));
}

/*===========================================FUNC====================================================*/
void add_timer(int obj_id, OPMODE ev_type, system_clock::time_point t)
{
	g_timer_lock.lock();
	g_timer_queue.emplace(obj_id, t, ev_type);
	g_timer_lock.unlock();
}

bool CAS(atomic<STATUS>* addr, STATUS* old_v, STATUS new_v)
{
	return atomic_compare_exchange_strong(addr, old_v, new_v);
}

void Check_Korean(const char* text, int* len)
{
	// 한글은 한 글자당 2Byte -> 각 Byte의 최상위 bit가 1이다.
	for (int i = 0; i < MAX_STR_LEN; ++i)
	{
		if (text[i] == '\0') return;

		// 한글이 아닐 경우
		if ((text[i] & 0x80) != 0x80)
			*len += 1;
		// 한글일 경우
		else if ((text[i] & 0x80) == 0x80)
			*len += 2;
	}
}
