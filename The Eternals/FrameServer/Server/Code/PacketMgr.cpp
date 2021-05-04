#include "stdafx.h"
#include "PacketMgr.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"

/* ========================��Ŷ ó��========================*/
void process_packet(int id)
{
	/* Server Number �ش� ���� */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	// ��Ŷ Ÿ��
	char p_type = pPlayer->m_packet_start[1];

	switch (p_type)
	{
	case CS_LOGIN:
	{
		cs_packet_login* p = reinterpret_cast<cs_packet_login*>(pPlayer->m_packet_start);

		pPlayer->Get_ClientLock().lock();
		strncpy_s(pPlayer->m_ID, p->name, strlen(p->name));
		pPlayer->m_type = p->o_type;
		pPlayer->Get_ClientLock().unlock();

#ifndef DUMMY
		/* CHECK ID in Database Server */
		if (false == CDBMgr::GetInstance()->Check_ID(id, p->password))
		{
			/* ó�� ���ӿ� ������ ������� ȸ������ ��� in Database Server */
			CDBMgr::GetInstance()->Insert_NewPlayer_DB(id, p->password);
		}
#endif // DUMMY

		/* �α��� ���� ��Ŷ ���� */
		send_login_ok(id);

		/* Sector �ٽ� ��� (���� �� �̸� �� �� �ϰ�����. �������� ���� �� �� �� ���(sector�� Key�� Unique��) */
		CSectorMgr::GetInstance()->Enter_ClientInSector(id, (int)(pPlayer->m_vPos.z / SECTOR_SIZE), (int)(pPlayer->m_vPos.x / SECTOR_SIZE));

		/* �ش� �÷��̾ ��ϵǾ� �ִ� ���� ���� �����鿡�� ������ �˸� */
		unordered_set<pair<int, int>> nearSector;
		nearSector.reserve(5);
		CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

		// ���� ���� ��ȸ
		for (auto& s : nearSector)
		{
			// ���� ���� ���� Ÿ �������� �ִ��� �˻�
			if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
			{
				// Ÿ ������ ���� ��ȣ ����
				for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
				{
					// '��'���� ���� ��Ŷ ���� X
					if (obj_num == id) continue;

					/* Ÿ������ ��� ó�� */
					if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
					{
						CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

						// ������ �����鿡�Ը� ���� ���Ḧ �˸�
						if (pOther->Get_IsConnected())
						{
							// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
							if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pOther)) continue;
							
							/* Ÿ ������ �þ߿� ���� ���� ��� '��'�� ��Ͻ�Ŵ */
							pOther->v_lock.lock();
							if (0 == pOther->view_list.count(id))
							{
								pOther->view_list.insert(id);
								pOther->v_lock.unlock();
								send_enter_packet(obj_num, id);
							}
							else pOther->v_lock.unlock();

							/* '��'�� �þ߿� Ÿ ������ ���� ��� -> Ÿ ���� ��� */
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
					/* NPC�� ��� ó�� */
					else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
					{
						CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

						// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
						if (false == CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC)) continue;
						pPlayer->v_lock.lock();
						pPlayer->view_list.insert(obj_num);
						pPlayer->v_lock.unlock();
						pNPC->send_NPC_enter_packet(id);
					}
					/* MONSTER�� ��� ó��*/
					else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
					{
						CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

						// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X
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

		process_attack(id, vDir, vPos, iAniIdx, fEndAngleY); // ��ų or �⺻ ���� �ִϸ��̼� �ε��� �ο� �ʿ���
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
		break;
	case CS_LOGOUT:
		break;

	case CS_COLLIDE: 
	{
		cs_packet_player_collision* p = reinterpret_cast<cs_packet_player_collision*>(pPlayer->m_packet_start);
		process_collide(id, p->col_id);
	}
	break;

	case CS_COLLIDE_MONSTER:
	{
		cs_packet_player_collision* p = reinterpret_cast<cs_packet_player_collision*>(pPlayer->m_packet_start);

		CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", p->col_id));
		if (nullptr == pMonster) return;

		pMonster->Hurt_Monster(id, pPlayer->m_iAtt);
	}
	break;

	case CS_STAGE_CHANGE:
	{
		cs_packet_stage_change* p = reinterpret_cast<cs_packet_stage_change*>(pPlayer->m_packet_start);
		process_stage_change(id, p->stage_id);
	}
	break;
	}
}
/* ========================��Ŷ ������========================*/
void process_recv(int id, DWORD iosize)
{
	/* Server Number �ش� ���� */
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;
	
	// m_packet_start		: ó���� ������ ���� �� ��ġ (= iocp_buffer)
	// m_packet_start[0]	: ó���� ��Ŷ�� ũ��
	unsigned char p_size = pPlayer->m_packet_start[0];
	
	/* ���� ��Ŷ�� ���� ������ ��ġ ���� */
	// m_recv_start			: ������ ��Ŷ�� ���� ��ġ
	unsigned char* next_recv_ptr = pPlayer->m_recv_start + iosize;

	// ó���� ��Ŷ ũ�⺸�� �����ִ� ������ ������ ��� ��Ŷ ó�� ����
	while (p_size <= next_recv_ptr - pPlayer->m_packet_start)
	{
		// ��Ŷ ó�� (= ��Ŷ �����)
		process_packet(id);
		// ��Ŷ ó�� �� ������ ������ ó�� ���� ���� ��ġ ����
		pPlayer->m_packet_start += p_size;
		
		// recv�� �����͸� ó���� �ߴµ� ���� ó������ ���� �����͸� ������ ���� ���� �ִ�.
		// ó���ؾ� �� ��Ŷ ũ�� ����
		if (pPlayer->m_packet_start < next_recv_ptr)
			p_size = pPlayer->m_packet_start[0];
		else
			break;
	} 

	// ó������ ����(���� �ִ�) �������� ���� ��ġ 
	long long left_data = next_recv_ptr - pPlayer->m_packet_start;

	// Recv ó�� �� -> �ٽ� Recv�� �ؾ� �Ѵ�.
	// �ٽ� Recv�� �ϱ� ���� �� ������ ���� ������ Ȯ�� �� �� ������� ��� �ʱ�ȭ
	// �����ִ� ������ MIN_BUUFER ���� ������ ���� �����͸� ������ �� ������ �Ű��ش�. 
	if ((MAX_BUFFER - (next_recv_ptr - pPlayer->m_recv_over.iocp_buf)) < MIN_BUFFER)
	{
		// ���� ������ ���۸� iocp_buf�� ����
		memcpy(pPlayer->m_recv_over.iocp_buf, pPlayer->m_packet_start, left_data);
		// ó���� ������ ���� ����
		pPlayer->m_packet_start = pPlayer->m_recv_over.iocp_buf;
		// ���� ��Ŷ�� ���� ������ ������ġ ����
		next_recv_ptr = pPlayer->m_packet_start + left_data;
	}

	// �� ���� ���� ���� ���� Ȯ�� �� ���� Recv �غ��ϱ�
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

	p.o_type	= pPlayer->m_type;
	p.level		= pPlayer->m_iLevel;
	p.hp		= pPlayer->m_iHp;
	p.maxHp		= pPlayer->m_iMaxHp;
	p.mp		= pPlayer->m_iMp;
	p.maxMp		= pPlayer->m_iMaxMp;
	p.exp		= pPlayer->m_iExp;
	p.maxExp	= pPlayer->m_iMaxExp;
	p.att		= pPlayer->m_iAtt;
	p.spd		= pPlayer->m_fSpd;

	p.posX		= pPlayer->m_vPos.x;
	p.posY		= pPlayer->m_vPos.y;
	p.posZ		= pPlayer->m_vPos.z;

	send_packet(id, &p);
}

void send_enter_packet(int to_client, int new_id)
{
	sc_packet_enter p;

	CPlayer* pNewPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", new_id));

	p.size = sizeof(p);
	p.type = SC_PACKET_ENTER;
	p.id = new_id;

	pNewPlayer->Get_ClientLock().lock();
	strncpy_s(p.name, pNewPlayer->m_ID, strlen(pNewPlayer->m_ID));

	pNewPlayer->Get_ClientLock().unlock();
	//p.o_type = pNewPlayer->m_type;
	p.o_type = PC_GLADIATOR;
	p.stageID = pNewPlayer->m_chStageId;

	p.posX = pNewPlayer->m_vPos.x;
	p.posY = pNewPlayer->m_vPos.y;
	p.posZ = pNewPlayer->m_vPos.z;

	p.dirX = pNewPlayer->m_vDir.x;
	p.dirY = pNewPlayer->m_vDir.y;
	p.dirZ = pNewPlayer->m_vDir.z;

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

	p.size = sizeof(p);
	p.type = SC_PACKET_STAT_CHANGE;

	p.id = id;
	p.hp = pPlayer->m_iHp;
	p.mp = pPlayer->m_iMp;
	p.exp = pPlayer->m_iExp;

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

void process_move(int id, const _vec3& _vDir, const _vec3& _vPos)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;	
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���⺤�Ϳ� ��ġ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;
	
	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾��� �̷� ��ġ ��ǥ ���� -> �̷� ��ġ��ǥ�� �ӽ� ������ ���� */
	_vec2 coll_pos = _vec2(0.f);

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos, pPlayer->m_chStageId))
	{
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;
	}

	send_move_packet(id, id);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// �ش� �÷��̾��� ���ŵ� �þ� ���
	unordered_set<int> new_viewlist;

	// ���� ���� ��ȸ
	for (auto& s : nearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));
					
					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pOther->Get_IsConnected()) continue;
					
					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);			
				}
				/* NPC�� ��� ó�� */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (int server_num : new_viewlist)
	{
		// �÷��̾� �þ� ��Ͽ� ���� ���� ��ü ó�� (���� �þ� ��Ͽ� ���ٸ�)
		if (0 == old_viewlist.count(server_num))
		{
			// �÷��̾� �þ� ��Ͽ� �߰�
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// ���ο� Ÿ������ �þ� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
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
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// �÷��̾� �þ� ��Ͽ� ��� �ִ� ��ü ó��
		else
		{
			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ��� �ִ� ���
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_move_packet(server_num, id);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // ���濡�� ���� ���� ���� ���� .... ���߿�
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (int s_num : old_viewlist)
	{
		// ���ŵ� �þ� ��Ͽ� ���� ��ü�� ���
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();

				// Ÿ ������ �þ߿� '��'�� �ִٸ� ����
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
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "move" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_move_stop(int id, const _vec3& _vPos, const _vec3& _vDir)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;

	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾�κ��� ���� ���� ��ġ�� ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;
	
	send_move_stop_packet(id, id);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// �ش� �÷��̾��� ���ŵ� �þ� ���
	unordered_set<int> new_viewlist;

	// ���� ���� ��ȸ
	for (auto& s : nearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pOther->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);
				}
				/* NPC�� ��� ó�� */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (int server_num : new_viewlist)
	{
		// �÷��̾� �þ� ��Ͽ� ���� ���� ��ü ó�� (���� �þ� ��Ͽ� ���ٸ�)
		if (0 == old_viewlist.count(server_num))
		{
			// �÷��̾� �þ� ��Ͽ� �߰�
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// ���ο� Ÿ������ �þ� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
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
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// �÷��̾� �þ� ��Ͽ� ��� �ִ� ��ü ó��
		else
		{
			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ��� �ִ� ���
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_move_stop_packet(server_num, id);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // ���濡�� ���� ���� ���� ���� .... ���߿�
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (int s_num : old_viewlist)
	{
		// ���ŵ� �þ� ��Ͽ� ���� ��ü�� ���
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();

				// Ÿ ������ �þ߿� '��'�� �ִٸ� ����
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
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "move_stop" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_collide(int id, int colID)
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
			pPlayer->m_iHp -= pMonster->m_iAtt;
		else
		{
			/* Player Dead */
			pPlayer->m_iHp = 0;
			pPlayer->Set_IsDead(true);
		}

		/* �ش� �÷��̾��� �þ� ��� */
		pPlayer->v_lock.lock();
		unordered_set<int> viewlist = pPlayer->view_list;
		pPlayer->v_lock.unlock();

		/* �ش� �������� �ٲ� stat ���� */
		send_player_stat(id, id);

		/* �þ� ��� ���� ��ü ó�� */
		for (int server_num : viewlist)
		{
			if (server_num == id) continue;
			// �þ� ���� �ٸ� �����鿡�� �ٲ� ���� ����
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				send_player_stat(server_num, id);
			}
		}
	}
}

void process_attack(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx, float end_angleY)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;
	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���⺤�Ϳ� ��ġ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾��� �̷� ��ġ ��ǥ ���� -> �̷� ��ġ��ǥ�� �ӽ� ������ ���� */
	_vec2 coll_pos = _vec2(0.f);

	if (CCollisionMgr::GetInstance()->Is_DeadReckoning(pPlayer->m_vPos, pPlayer->m_vDir, &coll_pos, pPlayer->m_chStageId))
	{
		pPlayer->m_vTempPos.x = coll_pos.x;
		pPlayer->m_vTempPos.y = 0.f;
		pPlayer->m_vTempPos.z = coll_pos.y;
	}

	send_attack_packet(id, id, aniIdx, end_angleY);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// �ش� �÷��̾��� ���ŵ� �þ� ���
	unordered_set<int> new_viewlist;

	// ���� ���� ��ȸ
	for (auto& s : nearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pOther->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);
				}
				/* NPC�� ��� ó�� */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (int server_num : new_viewlist)
	{
		// �÷��̾� �þ� ��Ͽ� ���� ���� ��ü ó�� (���� �þ� ��Ͽ� ���ٸ�)
		if (0 == old_viewlist.count(server_num))
		{
			// �÷��̾� �þ� ��Ͽ� �߰�
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// ���ο� Ÿ������ �þ� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
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
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// �÷��̾� �þ� ��Ͽ� ��� �ִ� ��ü ó��
		else
		{
			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ��� �ִ� ���
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_attack_packet(server_num, id, aniIdx, end_angleY);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // ���濡�� ���� ���� ���� ���� .... ���߿�
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (int s_num : old_viewlist)
	{
		// ���ŵ� �þ� ��Ͽ� ���� ��ü�� ���
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();

				// Ÿ ������ �þ߿� '��'�� �ִٸ� ����
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
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "attack" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_attack_stop(int id, const _vec3& _vDir, const _vec3& _vPos, int aniIdx)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� ��ġ�� & ����� ��ġ�� */
	float ori_x, ori_y, ori_z;

	ori_x = pPlayer->m_vPos.x;
	ori_y = pPlayer->m_vPos.y;
	ori_z = pPlayer->m_vPos.z;

	/* �ش� �÷��̾��� ���� �þ� ��� */
	pPlayer->v_lock.lock();
	unordered_set<int> old_viewlist = pPlayer->view_list;
	pPlayer->v_lock.unlock();

	/* �ش� �÷��̾�κ��� ���� ���� ��ġ�� ���� */
	pPlayer->m_vDir = _vDir;
	pPlayer->m_vPos = _vPos;

	send_attack_stop_packet(id, id, aniIdx);

	/* ����� ��ǥ�� ���� ���� */
	CSectorMgr::GetInstance()->Compare_exchange_Sector(id, (int)ori_z, (int)ori_x, (int)(pPlayer->m_vPos.z), (int)(pPlayer->m_vPos.x));

	/* �÷��̾� �ֺ��� ���͸� �˻� -> ������ ���� ���� ��ü���� ��ǥ ���� */
	unordered_set<pair<int, int>> nearSector;
	nearSector.reserve(5);
	CSectorMgr::GetInstance()->Get_NearSectorIndex(&nearSector, (int)pPlayer->m_vPos.x, (int)pPlayer->m_vPos.z);

	// �ش� �÷��̾��� ���ŵ� �þ� ���
	unordered_set<int> new_viewlist;

	// ���� ���� ��ȸ
	for (auto& s : nearSector)
	{
		// ���� ���� ���� Ÿ �������� �ִ��� �˻�
		if (!(CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList().empty()))
		{
			// Ÿ ������ ���� ��ȣ ����
			for (auto obj_num : CSectorMgr::GetInstance()->Get_SectorList()[s.first][s.second].Get_ObjList())
			{
				/* Ÿ������ ��� ó�� */
				if (obj_num == id) continue;
				if (true == CObjMgr::GetInstance()->Is_Player(obj_num))
				{
					CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", obj_num));

					// ������ ������ �þ� ��Ͽ� ����Ѵ�.
					if (!pOther->Get_IsConnected()) continue;

					// �þ� ���� �ִٸ� �þ� ��Ͽ� ����Ѵ�.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pOther))
						new_viewlist.insert(obj_num);
				}
				/* NPC�� ��� ó�� */
				else if (true == CObjMgr::GetInstance()->Is_NPC(obj_num))
				{
					CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pNPC))
					{
						new_viewlist.insert(obj_num);
						pNPC->wakeUp_npc();
					}
				}
				/* MONSTER�� ��� ó��*/
				else if (true == CObjMgr::GetInstance()->Is_Monster(obj_num))
				{
					CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", obj_num));

					// �þ� ���� ���ٸ� �þ� ��Ͽ� ���X.
					if (CObjMgr::GetInstance()->Is_Near(pPlayer, pMonster))
					{
						new_viewlist.insert(obj_num);
						pMonster->active_monster();
					}
				}
			}
		}
	}

	/* ���ο� �þ� ��� ���� ��ü ó�� */
	for (int server_num : new_viewlist)
	{
		// �÷��̾� �þ� ��Ͽ� ���� ���� ��ü ó�� (���� �þ� ��Ͽ� ���ٸ�)
		if (0 == old_viewlist.count(server_num))
		{
			// �÷��̾� �þ� ��Ͽ� �߰�
			pPlayer->v_lock.lock();
			pPlayer->view_list.insert(server_num);
			pPlayer->v_lock.unlock();

			// ���ο� Ÿ������ �þ� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				// �÷��̾�('��')���� ���ο� ���� ���� ��Ŷ ����
				send_enter_packet(id, server_num);

				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
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
			// ���� �þ߿� ���� NPC�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_NPC(server_num))
			{
				// �÷��̾�('��')���� NPC���� ��Ŷ ����
				CNpc* pNPC = static_cast<CNpc*>(CObjMgr::GetInstance()->Get_GameObject(L"NPC", server_num));
				pNPC->send_NPC_enter_packet(id);
			}
			// ���� �þ߿� ���� MONSTER�� ��� ó��
			else if (true == CObjMgr::GetInstance()->Is_Monster(server_num))
			{
				// �÷��̾�('��')���� Monster ���� ��Ŷ ����
				CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", server_num));
				pMonster->send_Monster_enter_packet(id);
			}
		}
		// �÷��̾� �þ� ��Ͽ� ��� �ִ� ��ü ó��
		else
		{
			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(server_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", server_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();
				// Ÿ ������ �þ� ��Ͽ� '��'�� ��� �ִ� ���
				if (0 != pOther->view_list.count(id))
				{
					pOther->v_lock.unlock();
					send_attack_stop_packet(server_num, id, aniIdx);
				}
				// Ÿ ������ �þ� ��Ͽ� '��'�� ���� ���� ���
				else
				{
					pOther->view_list.insert(id);
					pOther->v_lock.unlock();
					send_enter_packet(server_num, id);
				}
			}
		} // ���濡�� ���� ���� ���� ���� .... ���߿�
	}

	/* ���� �þ� ��Ͽ��� ����� ��ü ó�� */
	for (int s_num : old_viewlist)
	{
		// ���ŵ� �þ� ��Ͽ� ���� ��ü�� ���
		if (0 == new_viewlist.count(s_num))
		{
			pPlayer->v_lock.lock();
			pPlayer->view_list.erase(s_num);
			pPlayer->v_lock.unlock();
			send_leave_packet(id, s_num);

			// Ÿ ���� ó��
			if (true == CObjMgr::GetInstance()->Is_Player(s_num))
			{
				CPlayer* pOther = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", s_num));

				// Ÿ ������ �þ� ��� ó��
				pOther->v_lock.lock();

				// Ÿ ������ �þ߿� '��'�� �ִٸ� ����
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
	/* �÷��̾� �þ� ���� �ִ� ��ü ��� */
	for (int server_obj : new_viewlist)
		cout << "attck_stop" << server_obj << "�þ� ���� �����մϴ�." << endl;
#endif
}

void process_stance_change(int id, const bool& stance)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(CObjMgr::GetInstance()->Get_GameObject(L"PLAYER", id));

	if (pPlayer == nullptr) return;

	/* �ش� �÷��̾��� ���� �þ� ��� */
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

	/* �ش� �÷��̾��� ���� �þ� ��� */
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

/*============================================MONSTER======================================================*/
//void active_monster(int id)
//{
//	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", id));
//
//	if (nullptr == pMonster) return;
//
//	/* Monster�� Ȱ��ȭ�Ǿ� ���� ���� ��� Ȱ��ȭ */
//	if (pMonster->m_status == ST_NONACTIVE)
//	{
//		STATUS prev_state = pMonster->m_status;
//		atomic_compare_exchange_strong(&pMonster->m_status, &prev_state, ST_ACTIVE);	
//	}
//}
//
//void nonActive_monster(int id)
//{
//	CMonster* pMonster = static_cast<CMonster*>(CObjMgr::GetInstance()->Get_GameObject(L"MONSTER", id));
//
//	if (nullptr == pMonster) return;
//
//	if (pMonster->m_status != ST_NONACTIVE)
//	{
//		STATUS prev_state = pMonster->m_status;
//		if (true == atomic_compare_exchange_strong(&pMonster->m_status, &prev_state, ST_NONACTIVE))
//			pMonster->m_vTempPos = pMonster->m_vPos;
//	}
//}

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
